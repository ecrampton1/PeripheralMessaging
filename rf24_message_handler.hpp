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
static bool publish_message(uint8_t* buffer, const size_t length, uint8_t node=0);
static void serviceOnce();

private:
//static void process_messages(uint8_t* buffer,const size_t length);
static constexpr int BUFFER_SIZE = 32;
static uint8_t mBuffer[BUFFER_SIZE];
static void handle_version_query(void* args, void* msg,uint16_t calling_id);
};

}
#endif //RF24_MESSAGING_HANDLER_HPP_
