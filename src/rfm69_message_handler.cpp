#include "rfm69_message_handler.hpp"
#ifdef __arm__
#include "spi_wrapper.hpp"
#include "gpio_wrapper.hpp"
#include "sys_wrapper.hpp"
#include "debug_wrapper.hpp"
#include "rfm69/rfm69.hpp"
#include <iostream>

using DIO0 = GpioWrapper<4>;
using CS = GpioWrapper<1>;
using RF = Rfm69< SpiDev0, SysWrapper,  CS, DIO0, CarrierFrequency::FREQUENCY_915,DebugWrapper>;
#else
#include "mcu_config.hpp"
#define ENABLE_HEARTBEAT
#endif

namespace PeripheralMessages
{

uint8_t RFM69Handler::mHandlerBuffer[BUFFER_SIZE];
bool RFM69Handler::mSentUpdate = false;


void RFM69Handler::begin(const uint8_t node,const uint8_t network)
{
	RF::init();
	RF::setNodeAddress(node);
	RF::setNetworkAddress(network);
	RF::enableRx();

}


void RFM69Handler::serviceOnce()
{

	while (RF::isPayloadReady() ) {
		std::cout << "PLR" << std::endl;
		uint8_t length = RF::readPayload(mHandlerBuffer,sizeof(mHandlerBuffer));
		RF::enableRx();
		PacketHeader* header = reinterpret_cast<PacketHeader*>(mHandlerBuffer);
		uint8_t *msg = &mHandlerBuffer[sizeof(PacketHeader)];
		MessageHandler::process_messages(msg,length - sizeof(PacketHeader),header->Source);
	}

	//Always connected nodes this works
	//TODO low power nodes how to handle heartbeat
#ifndef __arm__
	serviceHeartbeat();
#endif
}


bool RFM69Handler::publishMessage(const MessageBuffer& buffer,const  uint16_t node)
{
	bool ret = RF::writePayload(buffer.mBuffer,buffer.mSize,node);
	RF::enableRx();
	return ret;
}

void RFM69Handler::handleVersionQuery(void* args, void* msg,const uint16_t calling_id)
{
	//PRINT(__FUNCTION__, ENDL)
	VersionDataMsg version;
	version.get_message_payload()->major = VERSION_MAJOR;
	version.get_message_payload()->minor = VERSION_MINOR;
	//publish_message(buffer,sizeof(buffer),calling_id);
	publishMessage(version , calling_id);
}

#ifndef __arm__
//Should send an update once every 2 seconds
void RFM69Handler::serviceHeartbeat()
{
	uint8_t buffer[8];
	uint32_t upTime = sys::updateUpTimeInSeconds();
	//detect the edge of change
	if(false == mSentUpdate && (upTime & 0x01)) {
		HeartbeatDataMsg heartbeat(mHandlerBuffer,BUFFER_SIZE,true);
		heartbeat.get_message_payload()->upTimeInSeconds = upTime;
		publishMessage(heartbeat, GATEWAY_ID);
		mSentUpdate = true;
	}
	mSentUpdate = (upTime & 0x01);
}
#endif

}
