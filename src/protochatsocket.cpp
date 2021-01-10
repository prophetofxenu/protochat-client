#include "src/include/protochatsocket.h"

#include <iostream>
#include <string>
using std::string;
#include <vector>
using std::vector;

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <cryptopp/osrng.h>
using CryptoPP::AutoSeededRandomPool;
using CryptoPP::AutoSeededX917RNG;
#include <cryptopp/aes.h>
using CryptoPP::AES;
#include <cryptopp/eccrypto.h>
using CryptoPP::ECP;
using CryptoPP::ECDH;
using CryptoPP::ECDSA;
using CryptoPP::DL_GroupParameters_EC;
using CryptoPP::DL_PublicKey_EC;
using CryptoPP::SignatureVerificationFilter;
#include <cryptopp/secblock.h>
using CryptoPP::SecByteBlock;
using CryptoPP::byte;
using CryptoPP::ByteQueue;
using CryptoPP::BufferedTransformation;
using CryptoPP::StringSource;
using CryptoPP::ArraySink;
#include <cryptopp/oids.h>
using CryptoPP::OID;
#include <cryptopp/asn.h>
using namespace CryptoPP::ASN1;
#include <cryptopp/integer.h>
using CryptoPP::Integer;
#include <cryptopp/hex.h>
using CryptoPP::HexEncoder;
#include <cryptopp/hkdf.h>
using CryptoPP::HKDF;
#include <cryptopp/sha.h>
using CryptoPP::SHA256;


const OID ProtochatSocket::CURVE = secp384r1();
constexpr byte ProtochatSocket::HKDF_SALT[];
constexpr byte ProtochatSocket::HKDF_INFO[];
constexpr byte ProtochatSocket::CIP_HEADER[];
constexpr char ProtochatSocket::VERIFY_PHRASE[];
constexpr char ProtochatSocket::CONFIRM_PHRASE[];


ProtochatSocket::ProtochatSocket(string &addr, int port) :
    addr(addr), port(port) {}


ProtochatSocket::~ProtochatSocket() {
    disconnect();
}


bool ProtochatSocket::connected() {
    return sockfd > -1;
}


bool ProtochatSocket::connect() {
    goto connect_start;

connect_fail:
    std::cerr << "Error during handshake" << std::endl;
    if (sockfd > -1) {
        close(sockfd);
        sockfd = -1;
    }
    return false;

connect_start:

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, addr.c_str(), &serv_addr.sin_addr);
    if (::connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Unable to connect" << std::endl;
        close(sockfd);
        sockfd = -1;
        return false;
    }

    // receive public key from server
    int x_len, y_len;
    if (static_cast<ulong>(read(sockfd, &x_len, sizeof(int))) < sizeof(int))
        goto connect_fail;
    if (static_cast<ulong>(read(sockfd, &y_len, sizeof(int))) < sizeof(int))
        goto connect_fail;
    char x_char[x_len], y_char[y_len];
    if (read(sockfd, x_char, x_len) < x_len)
        goto connect_fail;
    if (read(sockfd, y_char, y_len) < y_len)
        goto connect_fail;
    Integer x(x_char), y(y_char);

    // generate our pieces
    AutoSeededX917RNG<AES> rng;
    ECDH<ECP>::Domain dh(CURVE);
    SecByteBlock priv(dh.PrivateKeyLength()), pub(dh.PublicKeyLength());
    dh.GenerateKeyPair(rng, priv, pub);
    SecByteBlock shared(dh.AgreedValueLength());

    // send public key to server
    string pub_string;
    HexEncoder pub_key_encoder;
    pub_key_encoder.Put(pub, pub.size());
    pub_key_encoder.MessageEnd();
    size_t encoder_size = pub_key_encoder.MaxRetrievable();
    if (encoder_size > 0) {
        pub_string.resize(encoder_size);
        pub_key_encoder.Get((byte *) &pub_string[0], pub_string.size());
        int key_size = pub_string.size();
        if (static_cast<ulong>(::send(sockfd, &key_size, sizeof(int), 0)) < sizeof(int))
            goto connect_fail;
        if (::send(sockfd, pub_string.c_str(), key_size, 0) < key_size)
            goto connect_fail;
    } else {
        std::cerr << "Unable to complete handshake: error during ECDH keygen" << std::endl;
        close(sockfd);
        sockfd = -1;
        return false;
    }

    // agree to server key
    DL_GroupParameters_EC<ECP> params(CURVE);
    size_t params_size = params.GetEncodedElementSize(true);
    vector<byte> serverPublicKey(params_size);
    ECP::Point serverPoint(x, y);
    params.EncodeElement(true, serverPoint, &serverPublicKey[0]);
    dh.Agree(shared, priv, &serverPublicKey[0]);

    // derive stream key
    HKDF<SHA256> hkdf;
    byte derived_key[KEY_LEN];
    hkdf.DeriveKey(derived_key, KEY_LEN, shared.BytePtr(), shared.SizeInBytes(),
                   HKDF_SALT, HKDF_SALT_LEN, HKDF_INFO, HKDF_INFO_LEN);

    // setup stream cipher
    if (read(sockfd, iv, IV_LEN) < IV_LEN)
        goto connect_fail;

    enc.SetKeyWithIV(derived_key, KEY_LEN, iv, IV_LEN);
    dec.SetKeyWithIV(derived_key, KEY_LEN, iv, IV_LEN);

    // send handshake phrase
    byte verify_ct[VERIFY_PHRASE_LEN];
    byte mac[MAC_LEN];
    enc.EncryptAndAuthenticate(verify_ct, mac, MAC_LEN, iv, IV_LEN,
                               CIP_HEADER, CIP_HEADER_LEN,
                               (const byte *) VERIFY_PHRASE, VERIFY_PHRASE_LEN);
    if (::send(sockfd, verify_ct, VERIFY_PHRASE_LEN, 0) < VERIFY_PHRASE_LEN)
        goto connect_fail;
    if (::send(sockfd, mac, MAC_LEN, 0) < MAC_LEN)
        goto connect_fail;

    // receive confirmation phrase
    byte confirm_ct[CONFIRM_PHRASE_LEN];
    if (read(sockfd, confirm_ct, CONFIRM_PHRASE_LEN) < CONFIRM_PHRASE_LEN)
        goto connect_fail;
    if (read(sockfd, mac, MAC_LEN) < MAC_LEN)
        goto connect_fail;
    char confirm_pt[CONFIRM_PHRASE_LEN + 1];
    if (!dec.DecryptAndVerify((byte *) confirm_pt, mac, MAC_LEN, iv, IV_LEN,
                         CIP_HEADER, CIP_HEADER_LEN,
                         confirm_ct, CONFIRM_PHRASE_LEN))
        goto connect_fail;
    confirm_pt[CONFIRM_PHRASE_LEN] = '\0';
    string confirm_known(CONFIRM_PHRASE);
    string confirm_received(confirm_pt);
    if (confirm_known != confirm_received)
        goto connect_fail;

    return true;

}


void ProtochatSocket::disconnect() {
    if (sockfd == -1)
        return;
    close(sockfd);
    sockfd = -1;
}


bool ProtochatSocket::send(const byte *data, size_t len) {
    if (sockfd == -1)
        return false;

    byte ct[len];
    byte mac[MAC_LEN];
    enc.EncryptAndAuthenticate(ct, mac, MAC_LEN, iv, IV_LEN,
                               CIP_HEADER, CIP_HEADER_LEN,
                               data, len);
    size_t sent = 0;
    do
        sent += ::send(sockfd, ct + sent, len - sent, 0);
    while (sent < len);
    sent = 0;
    do
        sent += ::send(sockfd, mac + sent, MAC_LEN - sent, 0);
    while (sent < len);
    return true;
}


bool ProtochatSocket::receive(byte *buffer, size_t len) {
    if (sockfd == -1)
        return false;

    byte ct[len];
    byte mac[MAC_LEN];
    size_t received = 0;
    do
        received += read(sockfd, ct, len - received);
    while (received < len);
    received = 0;
    do
        received += read(sockfd, mac, MAC_LEN - received);
    while (received < MAC_LEN);
    return dec.DecryptAndVerify(buffer, mac, MAC_LEN, iv, IV_LEN,
                                CIP_HEADER, CIP_HEADER_LEN, ct, len);
}
