#include "message_handler.hpp"
#include <stdio.h>


namespace PeripheralMessages
{

#define FOR_ALL_INCOMING_MESSAGES(ACTION) \
	ACTION( VersionQuery )

void MessageHandler::process_messages(uint8_t* const buffer,const size_t length)
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
		 MESSAGE##Msg::trigger_callback(); \
		 break; \
		}

	FOR_ALL_INCOMING_MESSAGES(SWITCH_MESSAGE_HANDLER)

#undef SWITCH_MESSAGE_HANDLER
	default:
		break;
		//Unhandled messages dispose of since they are not used.
	}
}

}
