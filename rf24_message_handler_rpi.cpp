#include "rf24_message_handler_rpi.hpp"
#include <stdio.h>


namespace PeripheralMessages
{

static constexpr uint8_t NODEID = 0;




RF24MessageHandlerRpi::RF24MessageHandlerRpi() :
mRadio(22,0),
mNetwork(mRadio),
mMesh(mRadio,mNetwork)
{
	mMesh.setNodeID(0);
	// Connect to the mesh
	mMesh.begin(76, RF24_250KBPS);
	mRadio.printDetails();
	VersionQueryMsg::set_callback(&RF24MessageHandlerRpi::handle_version_query);
	VersionQueryMsg::set_callback_arguments(static_cast<void*>(this));
}


void RF24MessageHandlerRpi::service_once()
{
	mMesh.update();

	mMesh.DHCP();

	while(mNetwork.available()){
		RF24NetworkHeader header;
		mNetwork.peek(header);
		 

		if(header.type == USERTYPE) {
			size_t length = mNetwork.read(header,mBuffer,sizeof(mBuffer));
			MessageHandler::process_messages(mBuffer,length,mMesh.getNodeID(header.from_node));
		}
		else { //Else read it and throw it away
			mNetwork.read(header,0,0);
		}
	}
	delay(2);
}



bool RF24MessageHandlerRpi::publish_message(const MessageBuffer& buffer,const uint16_t node)
{
	bool ret = false;
	//if (!mMesh.write(node, buffer.mBuffer, USERTYPE, buffer.mSize)) {
	if (!mMesh.write(buffer.mBuffer, USERTYPE, buffer.mSize,node)) {
		printf("failed to write\n");
   } 
	else {
		printf("write passed\n");
		ret = true;
	}

	return ret;
}

void RF24MessageHandlerRpi::handle_version_query(void* args, void*  msg,const uint16_t calling_id)
{
	RF24MessageHandlerRpi* rpi_ptr = static_cast<RF24MessageHandlerRpi*>(args);
	VersionDataMsg version;
	version.get_message_payload()->major = VERSION_MAJOR;
	version.get_message_payload()->minor = VERSION_MINOR;
	rpi_ptr->publish_message(version,calling_id);
}

}
