#ifndef RF24_MESSAGING_HANDLER_HPP_
#define RF24_MESSAGING_HANDLER_HPP_
#include "message.hpp"
#include "message_handler.hpp"

namespace PeripheralMessages
{

class RF24MessageHandler
{
public:

static void begin();
static void initialize_incoming_messages();
static bool publish_message(const MessageBuffer& buffer, const uint16_t node=0);
static void serviceOnce();

private:

static constexpr int BUFFER_SIZE = 61;
static uint8_t mHandlerBuffer[BUFFER_SIZE];
static void handle_version_query(void* args, void* msg,const uint16_t calling_id);
};

}
#endif //RF24_MESSAGING_HANDLER_HPP_
