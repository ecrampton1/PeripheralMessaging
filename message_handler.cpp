#include "message_handler.hpp"
#include <stdio.h>


namespace PeripheralMessages
{

void MessageHandlerInterface::process_messages(const uint8_t* buffer,const size_t length)
{
	
	if(length < sizeof(PeripheralMessages::PayloadHeader)) {		
		return;
	}
	const PeripheralMessages::PayloadHeader* header = reinterpret_cast<const PeripheralMessages::PayloadHeader*>(buffer);
	
		
	switch(header->mMessageId) {
#define SWITCH_MESSAGE_HANDLER(MESSAGE) \
	case PeripheralMessages::MessageId::MESSAGE: \
		{ \
		 printf("Message is a %s\n", #MESSAGE); \
		 PeripheralMessages::MESSAGE##Msg msg; \
		 break; \
		} 

	FOR_ALL_INCOMING_MESSAGES(SWITCH_MESSAGE_HANDLER)

#undef SWITCH_MESSAGE_HANDLER
	default:
		printf("Default Handler id %d\n",header->mMessageId);
		break;
		//Unhandled messages dispose of since they are not used.
	}
}


void MessageHandlerInterface::publish_message()
{

}

}
