#include "rf24_message_handler.hpp"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include "messaging_config.hpp"
#include <stdio.h>


namespace PeripheralMessages
{

static constexpr uint8_t USERTYPE = 66;

uint8_t RF24MessageHandler::mHandlerBuffer[BUFFER_SIZE];


void RF24MessageHandler::begin()
{
	radio::begin();
  // Set the nodeID manually, ugh
	RF24Mesh::setNodeID(NODEID);
	//NOTE this should be in all message handlers MSP430 inheritance not working makes this less clean
	VersionQueryMsg::set_callback(&RF24MessageHandler::handle_version_query);
	//VersionQueryMsg::set_callback_arguments(this);
  // Connect to the mesh
	if(RF24Mesh::begin() == 0) {
		while(1);
	}
}


void RF24MessageHandler::serviceOnce()
{
	RF24Mesh::update();

	while (RF24Network::available()) {
		//P1OUT ^= 0x01;
		RF24NetworkHeader* header = reinterpret_cast<RF24NetworkHeader*>(mHandlerBuffer);
		RF24Network::peek(header);

		//If message is for us.
		if(header->type == USERTYPE) {
			uint8_t *msg = &mHandlerBuffer[sizeof(RF24NetworkHeader)];
			size_t length = RF24Network::read(header,msg,sizeof(mHandlerBuffer));
			MessageHandler::process_messages(msg,length,header->reserved);
		}
		else { //Else read it and throw it away

			RF24Network::read(header,0,0);
		}
	}
}



bool RF24MessageHandler::publish_message(const MessageBuffer& buffer,const  uint16_t node)
{
	bool ret = false;
	if (!RF24Mesh::write(buffer.mBuffer, USERTYPE, buffer.mSize, node)) {
      // If a write fails, check connectivity to the mesh network
      if ( ! RF24Mesh::checkConnection() ) {
        //refresh the network address
    	 RF24Mesh::renewAddress();
      } else {
			//connection is ok but send still failed
		}
    } 
	else {
		ret = true;
	}

	return ret;
}

void RF24MessageHandler::handle_version_query(void* args, void* msg,const uint16_t calling_id)
{
	PRINT(__FUNCTION__, ENDL)
	VersionDataMsg version;
	version.get_message_payload()->major = VERSION_MAJOR;
	version.get_message_payload()->minor = VERSION_MINOR;
	//publish_message(buffer,sizeof(buffer),calling_id);
	publish_message(version ,calling_id);
}

}
