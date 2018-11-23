#include "rfm69_message_handler.hpp"
#include "mcu_config.hpp"


namespace PeripheralMessages
{

uint8_t RFM69Handler::mHandlerBuffer[BUFFER_SIZE];


void RFM69Handler::begin(uint8_t node)
{
	rfm69::init();
	rfm69::enableRx();
	rfm69::setNodeAddress(node);
}


void RFM69Handler::serviceOnce()
{
	while (rfm69::isPayloadReady() ) {

		rfm69::readPayload(mHandlerBuffer,sizeof(mHandlerBuffer));

		PacketHeader* header = reinterpret_cast<PacketHeader*>(mHandlerBuffer);

		//If message is for us.
		MessageHandler::process_messages(mHandlerBuffer,header->Length,header->Source);
	}
}



bool RFM69Handler::publish_message(const MessageBuffer& buffer,const  uint16_t node)
{
	bool ret = rfm69::writePayloadWithAck(buffer.mBuffer,buffer.mSize,node);

	return ret;
}

void RFM69Handler::handle_version_query(void* args, void* msg,const uint16_t calling_id)
{
	//PRINT(__FUNCTION__, ENDL)
	VersionDataMsg version;
	version.get_message_payload()->major = VERSION_MAJOR;
	version.get_message_payload()->minor = VERSION_MINOR;
	//publish_message(buffer,sizeof(buffer),calling_id);
	publish_message(version , calling_id);
}

}
