#ifndef MESSAGING_HANDLER_HPP_
#define MESSAGING_HANDLER_HPP_
#include "message.hpp"

namespace PeripheralMessages
{

class MessageHandlerInterface
{
public:
MessageHandlerInterface() {}
virtual void process_messages(const uint8_t* buffer,const size_t length);
virtual void publish_message();
};

};
#endif //MESSAGING_HANDLER_HPP_
