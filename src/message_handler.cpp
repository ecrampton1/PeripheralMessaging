#include "message_handler.hpp"
#include "messaging_config.hpp"
#include <stdio.h>

namespace PeripheralMessages
{

#ifndef FOR_ALL_INCOMING_MESSAGES
#define FOR_ALL_INCOMING_MESSAGES ALL_MESSAGES \

#endif

void MessageHandler::process_messages(uint8_t* const buffer,const size_t length, uint16_t from_id)
{
	
	const PeripheralMessages::PayloadHeader* header = reinterpret_cast<const PeripheralMessages::PayloadHeader*>(buffer);
	
	if(length <= sizeof(PeripheralMessages::PayloadHeader)) {
		return;
	}	
	
switch(header->mMessageId) {
#define SWITCH_MESSAGE_HANDLER(MESSAGE) \
	case PeripheralMessages::MessageId::MESSAGE: \
		{ \
		 MESSAGE##Msg msg(buffer,length,false); \
		 if(msg.isMessageValid()) { \
		 	MESSAGE##Msg::trigger_callback(static_cast<void*>(&msg),from_id); \
		 } \
		 break; \
		}

	FOR_ALL_INCOMING_MESSAGES(SWITCH_MESSAGE_HANDLER)
	//Version query is handled internally
	//NOTE this means a handler above this needs to properly receive and send version data
#if FOR_ALL_INCOMING_MESSAGES!=ALL_MESSAGES
	SWITCH_MESSAGE_HANDLER( VersionQuery )
#endif

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
