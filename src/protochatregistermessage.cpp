#include "src/include/protochatregistermessage.h"

#include <cstring>


ProtochatRegisterMessage::ProtochatRegisterMessage() {}


ProtochatRegisterMessage::~ProtochatRegisterMessage() {}


std::string ProtochatRegisterMessage::getUsername() const {
    return username;
}


void ProtochatRegisterMessage::setUsername(const std::string &value) {
    username = value;
}


std::string ProtochatRegisterMessage::getPassword() const {
    return password;
}


void ProtochatRegisterMessage::setPassword(const std::string &value) {
    password = value;
}


std::string ProtochatRegisterMessage::getBio() const {
    return bio;
}


void ProtochatRegisterMessage::setBio(const std::string &value) {
    bio = value;
}


std::string ProtochatRegisterMessage::getProfilePicFilename() const {
    return profilePicFilename;
}


void ProtochatRegisterMessage::setProfilePicFilename(const std::string &value) {
    profilePicFilename = value;
}


std::vector<std::byte> ProtochatRegisterMessage::getProfilePic() const {
    return profilePic;
}


void ProtochatRegisterMessage::setProfilePic(const std::vector<std::byte> &value) {
    profilePic = value;
}


std::string ProtochatRegisterMessage::getProfilePicCaption() const {
    return profilePicCaption;
}


void ProtochatRegisterMessage::setProfilePicCaption(const std::string &value) {
    profilePicCaption = value;
}


bool ProtochatRegisterMessage::complete() {
    if (username.size() == 0)
        return false;
    if (password.size() == 0)
        return false;
    if (profilePicFilename.size() != 0 && profilePic.size() == 0)
        return false;
    return true;
}


size_t ProtochatRegisterMessage::messageSize() {
    if (!complete())
        return 0;
    size_t size = 0;
    size += sizeof(int) + username.size();
    size += sizeof(int) + password.size();
    if (bio.size() > 0)
        size += sizeof(int) + bio.size();
    else
        size += sizeof(int);
    if (profilePicFilename.size() > 0) {
        size += sizeof(int) + profilePicFilename.size();
        size += sizeof(int) + profilePic.size();
        size += sizeof(int) + profilePicCaption.size();
    } else
        size += sizeof(int);
    return size;
}


std::byte* ProtochatRegisterMessage::serialize() {
    if (!complete())
        return nullptr;
    std::byte *message = new std::byte[messageSize()];
    size_t pos = 0;
    int tmp = username.size();
    std::memcpy(message + pos, &tmp, sizeof(int));
    pos += sizeof(int);
    std::memcpy(message + pos, username.c_str(), tmp);
    pos += tmp;
    tmp = password.size();
    std::memcpy(message + pos, &tmp, sizeof(int));
    pos += sizeof(int);
    std::memcpy(message + pos, password.c_str(), tmp);
    pos += tmp;
    tmp = bio.size();
    std::memcpy(message + pos, &tmp, sizeof(int));
    pos += sizeof(int);
    if (bio.size() > 0) {
        std::memcpy(message + pos, bio.c_str(), tmp);
        pos += tmp;
    }
    tmp = profilePicFilename.size();
    std::memcpy(message + pos, &tmp, sizeof(int));
    pos += sizeof(int);
    if (profilePicFilename.size() > 0) {
        std::memcpy(message + pos, profilePicFilename.c_str(), tmp);
        pos += tmp;
        tmp = profilePic.size();
        std::memcpy(message + pos, &tmp, sizeof(int));
        pos += sizeof(int);
        for (int i = pos; i - pos < profilePic.size(); i++)
            message[i] = profilePic.at(i - pos);
        pos += profilePic.size();
        tmp = profilePicCaption.size();
        std::memcpy(message + pos, &tmp, sizeof(int));
        if (profilePicCaption.size() > 0) {
            pos += sizeof(int);
            std::memcpy(message + pos, profilePicCaption.c_str(), tmp);
        }
    }
    return message;
}
