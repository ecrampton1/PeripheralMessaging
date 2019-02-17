#include "rfm69_message_handler.hpp"
#include "mcu_config.hpp"


namespace PeripheralMessages
{

uint8_t RFM69Handler::mHandlerBuffer[BUFFER_SIZE];


void RFM69Handler::begin(uint8_t node)
{
	rfm69::init();
	rfm69::setNodeAddress(node);
	rfm69::setNetworkAddress(100);
	rfm69::enableRx();
}


void RFM69Handler::serviceOnce()
{
	while (rfm69::isPayloadReady() ) {
		PRINT("Ry\n")

		uint8_t length = rfm69::readPayload(mHandlerBuffer,sizeof(mHandlerBuffer));
		rfm69::enableRx();
		PacketHeader* header = reinterpret_cast<PacketHeader*>(mHandlerBuffer);
		uint8_t *msg = &mHandlerBuffer[sizeof(PacketHeader)];
		//If message is for us.
		MessageHandler::process_messages(msg,length - sizeof(PacketHeader),header->Source);
	}
}



bool RFM69Handler::publish_message(const MessageBuffer& buffer,const  uint16_t node)
{
	bool ret = rfm69::writePayloadWithAck(buffer.mBuffer,buffer.mSize,node);
	rfm69::enableRx();
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
