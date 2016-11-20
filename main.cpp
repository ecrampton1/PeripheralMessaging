#include <stdio.h>
#include <stdint.h>
#include "rf24_message_handler_rpi.hpp"


PeripheralMessages::RF24MessageHandlerRpi rpihandler;


void handle_ping(void* args, void*  msg, uint16_t calling_id)
{
	PeripheralMessages::PingPongQueryMsg* ping_ptr =
			static_cast<PeripheralMessages::PingPongQueryMsg*>(msg);
	printf("Ping received: %u\n", ping_ptr->get_message_payload()->count);

	PeripheralMessages::PingPongDataMsg pong;
	pong.get_message_payload()->count = ping_ptr->get_message_payload()->count;

	rpihandler.publish_message(pong.get_message_buffer(),pong.get_message_buffer_size(),calling_id);
}


int main()
{

	PeripheralMessages::PingPongQueryMsg::set_callback(&handle_ping);

	while(1)
	{
		rpihandler.service_once();
	}

}


