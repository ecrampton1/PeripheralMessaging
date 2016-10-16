#include <stdio.h>
#include <stdint.h>
#include "message.hpp"
#include "message_handler.hpp"

void handler(void* args)
{
	printf("Handler Called\n");
}

/*
uint8_t process_messages(uint8_t* buffer, const uint8_t size)
{
	if(size < sizeof(PayloadHeader)) { return size; }
	PayloadHeader* payload = static_cast<PayloadHeader*>(buffer);


}*/





int main()
{

	PeripheralMessages::VersionQueryMsg::set_callback(&handler,NULL);
	printf("Messaging test\n");
	
	/*processMessages(PeripheralMessages::MessageId::VersionQuery);
	processMessages(PeripheralMessages::MessageId::TemperatureQuery);
	processMessages(PeripheralMessages::MessageId::VersionData);*/
	PeripheralMessages::VersionQueryMsg msg;
	//processMessages(msg.get_message_buffer(),msg.get_message_buffer_size());
	
	PeripheralMessages::MessageHandlerInterface msg_handler;
	msg_handler.process_messages(msg.get_message_buffer(),msg.get_message_buffer_size());
	//msg.trigger_callback();

}


