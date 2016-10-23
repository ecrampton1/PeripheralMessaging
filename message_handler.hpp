#ifndef MESSAGING_HANDLER_HPP_
#define MESSAGING_HANDLER_HPP_
#include "message.hpp"

namespace PeripheralMessages
{

class MessageHandler
{
public:
static void process_messages(uint8_t* const buffer,const size_t length);
};

}
#endif //MESSAGING_HANDLER_HPP_
