#ifndef PROTOCHATREGISTERMESSAGE_H
#define PROTOCHATREGISTERMESSAGE_H

#include "src/include/protochatsockmessage.h"

#include <cstddef>
#include <fstream>
#include <string>
#include <vector>

class ProtochatRegisterMessage : public ProtochatSockMessage {

private:
    std::string username;
    std::string password;
    std::string bio;
    std::string profilePicFilename;
    std::vector<std::byte> profilePic;
    std::string profilePicCaption;

public:
    ProtochatRegisterMessage();
    ~ProtochatRegisterMessage();

    virtual bool complete() = 0;
    virtual size_t messageSize() = 0;
    virtual std::byte* serialize() = 0;

    std::string getUsername() const;
    void setUsername(const std::string &value);
    std::string getPassword() const;
    void setPassword(const std::string &value);
    std::string getBio() const;
    void setBio(const std::string &value);
    std::string getProfilePicFilename() const;
    void setProfilePicFilename(const std::string &value);
    std::vector<std::byte> getProfilePic() const;
    void setProfilePic(const std::vector<std::byte> &value);
    std::string getProfilePicCaption() const;
    void setProfilePicCaption(const std::string &value);
};

#endif // PROTOCHATREGISTERMESSAGE_H