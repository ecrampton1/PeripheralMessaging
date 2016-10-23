#include "rf24_message_handler.hpp"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <stdio.h>


namespace PeripheralMessages
{

static constexpr uint8_t NODEID = 10;
static constexpr uint8_t USERTYPE = 66;

uint8_t RF24MessageHandler::mBuffer[BUFFER_SIZE];

void RF24MessageHandler::begin()
{

	radio::begin();
  // Set the nodeID manually, ugh
	RF24Mesh::setNodeID(NODEID);
	//NOTE this should be in all message handlers MSP430 inheritance not working makes this less clean
	VersionQueryMsg::set_callback(&RF24MessageHandler::handle_version_query);

  // Connect to the mesh
	RF24Mesh::begin();
}


void RF24MessageHandler::serviceOnce()
{
	RF24Mesh::update();

	while (RF24Network::available()) {
		RF24NetworkHeader header;
		RF24Network::peek(header);

		//If message is for us.
		if(header.type == USERTYPE) {
			size_t length = RF24Network::read(header,mBuffer,sizeof(mBuffer));
			MessageHandler::process_messages(mBuffer,length,header.reserved);
		}
		else { //Else read it and throw it away
			RF24Network::read(header,0,0);
		}
	}
}



bool RF24MessageHandler::publish_message(uint8_t* buffer, const size_t length, uint8_t node)
{
	bool ret = false;
	if (!RF24Mesh::write(buffer, USERTYPE, length, node)) {

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

void RF24MessageHandler::handle_version_query(void*  msg, uint16_t calling_id)
{
	uint8_t buffer[sizeof(VersionMessage)+sizeof(PayloadHeader)];
	VersionDataMsg version(buffer,sizeof(buffer),true);
	version.get_message_payload()->major = VERSION_MAJOR;
	version.get_message_payload()->minor = VERSION_MINOR;
	publish_message(buffer,sizeof(buffer),calling_id);
}

}
