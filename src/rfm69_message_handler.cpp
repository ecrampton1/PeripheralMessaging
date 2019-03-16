#include "rfm69_message_handler.hpp"
#ifdef __arm__
#include "spi_wrapper.hpp"
#include "gpio_wrapper.hpp"
#include "sys_wrapper.hpp"
#include "debug_wrapper.hpp"
#include "rfm69/rfm69.hpp"

using DIO0 = GpioWrapper<4>;
using CS = GpioWrapper<1>;
using RF = Rfm69< SpiDev0, SysWrapper,  CS, DIO0, CarrierFrequency::FREQUENCY_915,DebugWrapper>;
#else
#include "mcu_config.hpp"
#endif

namespace PeripheralMessages
{

uint8_t RFM69Handler::mHandlerBuffer[BUFFER_SIZE];


void RFM69Handler::begin(uint8_t node)
{
	RF::init();
	RF::setNodeAddress(node);
	RF::setNetworkAddress(100);
	RF::enableRx();
}


void RFM69Handler::serviceOnce()
{
	while (RF::isPayloadReady() ) {
		
		uint8_t length = RF::readPayload(mHandlerBuffer,sizeof(mHandlerBuffer));
		RF::enableRx();
		PacketHeader* header = reinterpret_cast<PacketHeader*>(mHandlerBuffer);
		uint8_t *msg = &mHandlerBuffer[sizeof(PacketHeader)];
		MessageHandler::process_messages(msg,length - sizeof(PacketHeader),header->Source);
	}
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

}
