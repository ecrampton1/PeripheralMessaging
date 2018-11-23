#ifndef RFM69_MESSAGING_HANDLER_HPP_
#define RFM69_MESSAGING_HANDLER_HPP_
#include "message.hpp"
#include "message_handler.hpp"

namespace PeripheralMessages
{

class RFM69Handler
{
public:

static void begin(uint8_t node);
static void initialize_incoming_messages();
static bool publish_message(const MessageBuffer& buffer, const uint16_t node=0);
static void serviceOnce();

private:

static constexpr int BUFFER_SIZE = 32;
static uint8_t mHandlerBuffer[BUFFER_SIZE];
static void handle_version_query(void* args, void* msg,const uint16_t calling_id);
};

}
#endif //RFM69_MESSAGING_HANDLER_HPP_
