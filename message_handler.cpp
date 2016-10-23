#include "message_handler.hpp"
#include <stdio.h>


namespace PeripheralMessages
{

#define FOR_ALL_INCOMING_MESSAGES(ACTION) \
	ACTION( SwitchRequest )

void MessageHandler::process_messages(uint8_t* const buffer,const size_t length, uint16_t from_id)
{

	if(length < sizeof(PeripheralMessages::PayloadHeader)) {
		return;
	}
	const PeripheralMessages::PayloadHeader* header = reinterpret_cast<const PeripheralMessages::PayloadHeader*>(buffer);


switch(header->mMessageId) {
#define SWITCH_MESSAGE_HANDLER(MESSAGE) \
	case PeripheralMessages::MessageId::MESSAGE: \
		{ \
		 MESSAGE##Msg msg(buffer,length); \
		 MESSAGE##Msg::trigger_callback(static_cast<void*>(&msg),from_id); \
		 break; \
		}

	FOR_ALL_INCOMING_MESSAGES(SWITCH_MESSAGE_HANDLER)
	//Version query is handled internally
	//NOTE this means a handler above this needs to properly receive and send version data
	SWITCH_MESSAGE_HANDLER( VersionQuery )

#undef SWITCH_MESSAGE_HANDLER
	default:
		break;
		//Unhandled messages dispose of since they are not used.
	}
}

void MessageHandler::begin()
{

}


}