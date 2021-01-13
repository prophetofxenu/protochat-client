#ifndef PROTOCHATSOCKMESSAGE_H
#define PROTOCHATSOCKMESSAGE_H

#include <cstddef>
#include <cstdlib>

namespace Protochat::Messages {

    class SockMessage {

    public:
        virtual bool complete() = 0;
        virtual size_t messageSize() = 0;
        virtual std::byte* serialize() = 0;

    };

}

#endif // PROTOCHATSOCKMESSAGE_H
