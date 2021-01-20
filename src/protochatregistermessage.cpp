#include "src/include/protochatregistermessage.h"

#include <cstring>


Protochat::Messages::Register::Register() {}


Protochat::Messages::Register::~Register() {}


std::string Protochat::Messages::Register::getUsername() const {
    return username;
}


void Protochat::Messages::Register::setUsername(const std::string &value) {
    username = value;
}


std::string Protochat::Messages::Register::getPassword() const {
    return password;
}


void Protochat::Messages::Register::setPassword(const std::string &value) {
    password = value;
}


std::string Protochat::Messages::Register::getBio() const {
    return bio;
}


void Protochat::Messages::Register::setBio(const std::string &value) {
    bio = value;
}


std::string Protochat::Messages::Register::getProfilePicFilename() const {
    return profilePicFilename;
}


void Protochat::Messages::Register::setProfilePicFilename(const std::string &value) {
    profilePicFilename = value;
}


std::vector<std::byte> Protochat::Messages::Register::getProfilePic() const {
    return profilePic;
}


void Protochat::Messages::Register::setProfilePic(const std::vector<std::byte> &value) {
    profilePic = value;
}


std::string Protochat::Messages::Register::getProfilePicCaption() const {
    return profilePicCaption;
}


void Protochat::Messages::Register::setProfilePicCaption(const std::string &value) {
    profilePicCaption = value;
}


bool Protochat::Messages::Register::complete() {
    if (username.size() == 0)
        return false;
    if (password.size() == 0)
        return false;
    if (profilePicFilename.size() != 0 && profilePic.size() == 0)
        return false;
    return true;
}


size_t Protochat::Messages::Register::messageSize() {
    if (!complete())
        return 0;
    size_t size = 0;
    size += sizeof(u_char) + username.size();
    size += sizeof(u_char) + password.size();
    if (bio.size() > 0)
        size += sizeof(u_short) + bio.size();
    else
        size += sizeof(u_short);
    if (profilePicFilename.size() > 0) {
        size += sizeof(u_short) + profilePicFilename.size();
        size += sizeof(uint) + profilePic.size();
        size += sizeof(u_short) + profilePicCaption.size();
    } else
        size += sizeof(u_short);
    return size;
}


std::byte* Protochat::Messages::Register::serialize() {
    if (!complete())
        return nullptr;
    std::byte *message = new std::byte[messageSize()];
    size_t pos = 0;
    u_char ucTmp = username.size();
    std::memcpy(message + pos, &ucTmp, sizeof(u_char));
    pos += sizeof(u_char);
    std::memcpy(message + pos, username.c_str(), ucTmp);
    pos += ucTmp;
    ucTmp = password.size();
    std::memcpy(message + pos, &ucTmp, sizeof(u_char));
    pos += sizeof(u_char);
    std::memcpy(message + pos, password.c_str(), ucTmp);
    pos += ucTmp;
    u_short usTmp = bio.size();
    std::memcpy(message + pos, &usTmp, sizeof(u_short));
    pos += sizeof(u_short);
    if (bio.size() > 0) {
        std::memcpy(message + pos, bio.c_str(), usTmp);
        pos += usTmp;
    }
    usTmp = profilePicFilename.size();
    std::memcpy(message + pos, &usTmp, sizeof(u_short));
    pos += sizeof(u_short);
    if (profilePicFilename.size() > 0) {
        std::memcpy(message + pos, profilePicFilename.c_str(), usTmp);
        pos += usTmp;
        uint uiTmp = profilePic.size();
        std::memcpy(message + pos, &uiTmp, sizeof(uint));
        pos += sizeof(uint);
        for (int i = pos; i - pos < profilePic.size(); i++)
            message[i] = profilePic.at(i - pos);
        pos += profilePic.size();
        usTmp = profilePicCaption.size();
        std::memcpy(message + pos, &usTmp, sizeof(u_short));
        if (profilePicCaption.size() > 0) {
            pos += sizeof(u_short);
            std::memcpy(message + pos, profilePicCaption.c_str(), usTmp);
        }
    }
    return message;
}
