#ifndef CAPY_CHAT_CHATMESSAGE_H
#define CAPY_CHAT_CHATMESSAGE_H

#include <opendht.h>

class ChatMessage: public dht::ImMessage
{
public:
    using ImMessage::ImMessage;
    static const dht::ValueType TYPE;
};


#endif //CAPY_CHAT_CHATMESSAGE_H
