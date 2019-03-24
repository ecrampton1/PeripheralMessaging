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
	rfm69::printAllRegisters();
}


void RFM69Handler::serviceOnce()
{
	while (rfm69::isPayloadReady() ) {
		uint8_t length = rfm69::readPayload(mHandlerBuffer,sizeof(mHandlerBuffer));
		rfm69::enableRx();
		PacketHeader* header = reinterpret_cast<PacketHeader*>(mHandlerBuffer);
		uint8_t *msg = &mHandlerBuffer[sizeof(PacketHeader)];
		//If message is for us.
		MessageHandler::process_messages(msg,length - sizeof(PacketHeader),header->Source);
	}

	//Always connected nodes this works
	//TODO low power nodes how to handle heartbeat
	//serviceHeartbeat();
}


bool RFM69Handler::publishMessage(const MessageBuffer& buffer,const  uint16_t node)
{
	bool ret = rfm69::writePayloadWithAck(buffer.mBuffer,buffer.mSize,node);
	rfm69::enableRx();
	return ret;
}

void RFM69Handler::handleVersionQuuery(void* args, void* msg,const uint16_t calling_id)
{
	//PRINT(__FUNCTION__, ENDL)
	VersionDataMsg version;
	version.get_message_payload()->major = VERSION_MAJOR;
	version.get_message_payload()->minor = VERSION_MINOR;
	//publish_message(buffer,sizeof(buffer),calling_id);
	publishMessage(version , calling_id);
}

//Should send an update once every 2 seconds
void RFM69Handler::serviceHeartbeat()
{
	uint8_t buffer[8];
	uint32_t upTime = sys::updateUpTimeInSeconds();
	if(false == mSentUpdate) {
		HeartbeatDataMsg heartbeat(mHandlerBuffer,BUFFER_SIZE,true);
		heartbeat.get_message_payload()->upTimeInSeconds = upTime;
		publishMessage(heartbeat, GATEWAY_ID);
		mSentUpdate = true;
	}
	else {
		//send every other second
		if((upTime & 0x01) == 0) {
			mSentUpdate = false;
		}
	}
}

}
