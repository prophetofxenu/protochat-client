#ifndef PROTOCHATREGISTERMESSAGE_H
#define PROTOCHATREGISTERMESSAGE_H

#include "src/include/protochatsockmessage.h"

#include <cstddef>
#include <fstream>
#include <string>
#include <vector>

namespace Protochat::Messages {

    class Register : public Protochat::Messages::SockMessage {

    private:
        std::string username;
        std::string password;
        std::string bio;
        std::string profilePicFilename;
        std::vector<std::byte> profilePic;
        std::string profilePicCaption;

    public:
        Register();
        ~Register();

        virtual bool complete();
        virtual size_t messageSize();
        virtual std::byte* serialize();

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

}

#endif // PROTOCHATREGISTERMESSAGE_H
