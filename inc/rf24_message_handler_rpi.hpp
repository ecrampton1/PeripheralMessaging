#ifndef RF24_MESSAGING_HANDLER_HPP_
#define RF24_MESSAGING_HANDLER_HPP_
#include "message.hpp"
#include "message_handler.hpp"
#include "RF24Mesh/RF24Mesh.h"  
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>

namespace PeripheralMessages
{

class RF24MessageHandlerRpi
{
public:

RF24MessageHandlerRpi();
void initialize_incoming_messages();
bool publish_message(const MessageBuffer& buffer,const uint16_t node=0);
void service_once();

private:
//static void process_messages(uint8_t* buffer,const size_t length);
static constexpr int BUFFER_SIZE = 32;
uint8_t mBuffer[BUFFER_SIZE];
static void handle_version_query(void* args, void* msg,uint16_t calling_id);
RF24 mRadio;

//RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);  
RF24Network mNetwork;
RF24Mesh mMesh;

};

}
#endif //RF24_MESSAGING_HANDLER_HPP_
