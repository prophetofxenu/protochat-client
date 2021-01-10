#ifndef PROTOCHATSOCKET_H
#define PROTOCHATSOCKET_H

#include <string>
using std::string;

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>

#include <cryptopp/secblock.h>
#include <cryptopp/oids.h>
using CryptoPP::OID;
#include <cryptopp/asn.h>
#include <cryptopp/chachapoly.h>

class ProtochatSocket {

private:

    int sockfd = -1;
    string addr;
    int port;

    // crypto
    static const OID CURVE;
    static constexpr CryptoPP::byte HKDF_SALT[] = "PROTOCHAT HKDF SALT";
    static constexpr CryptoPP::byte HKDF_INFO[] = "PROTOCHAT HKDF INFO";
    static constexpr int HKDF_SALT_LEN = sizeof(HKDF_SALT) - 1;
    static constexpr int HKDF_INFO_LEN = sizeof(HKDF_INFO) - 1;
    static const int KEY_LEN = 32;
    static const int IV_LEN = 12;
    static const int MAC_LEN = 16;
    static constexpr CryptoPP::byte CIP_HEADER[] = "PROTOCHAT HEADER";
    static constexpr int CIP_HEADER_LEN = sizeof(CIP_HEADER) - 1;
    static constexpr char VERIFY_PHRASE[] = "PROTOCHAT VERIFY";
    static constexpr int VERIFY_PHRASE_LEN = sizeof(VERIFY_PHRASE) - 1;
    static constexpr char CONFIRM_PHRASE[] = "PROTOCHAT CONFIRM";
    static constexpr int CONFIRM_PHRASE_LEN = sizeof(CONFIRM_PHRASE) - 1;

    CryptoPP::byte iv[IV_LEN];
    CryptoPP::ChaCha20Poly1305::Encryption enc;
    CryptoPP::ChaCha20Poly1305::Decryption dec;

public:
    ProtochatSocket(string &addr, int port);
    bool connected();
    bool connect();
    void disconnect();
    bool send(const CryptoPP::byte *data, size_t len);
    bool receive(CryptoPP::byte *buffer, size_t len);
};

#endif // PROTOCHATSOCKET_H
