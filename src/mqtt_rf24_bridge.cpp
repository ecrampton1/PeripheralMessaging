#include "mqtt_rf24_bridge.hpp"
#include <string.h>


MqttRf24Bridge::MqttRf24Bridge()
{
	init();
}


void MqttRf24Bridge::handle_mqtt_topic(const struct mosquitto_message *message)
{
	if(strcmp(message->topic,RF_TOPIC_IN) == 0) {
		mIncomingTopics.push((const char*)message->payload);
	}
}



void MqttRf24Bridge::init()
{
#define INIT_MSG_HANDLERS(MSG) \
	PeripheralMessages::MSG##Msg::set_callback(&MqttRf24Bridge::handle_rf_message<PeripheralMessages::MSG##Msg>); \
	PeripheralMessages::MSG##Msg::set_callback_arguments(this);
	ALL_MESSAGES(INIT_MSG_HANDLERS)
#undef INIT_HANDLERS
	try {
		mMqttWrapper = std::unique_ptr<mqttwrapper>(new mqttwrapper(NULL,RF_TOPIC_IN,MQTT_IP,MQTT_PORT,true));
		mMqttWrapper->set_message_received_cb(std::bind(&MqttRf24Bridge::handle_mqtt_topic,this, std::placeholders::_1));
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}

void MqttRf24Bridge::service_once()
{
	//handle incoming topics
	if(!mIncomingTopics.empty()) {
		std::string message = mIncomingTopics.front();
		mqtt_to_rf24(message);
		mIncomingTopics.pop();
	}
	//handle incoming rf network messages
	mRPIHandler.service_once();
}

void MqttRf24Bridge::mqtt_to_rf24(std::string message)
{

	std::vector<std::string> substrs = split_mqtt_string(message);
	if(substrs.size() < 3) {
		return; //too small of a string to have any information
	}
	uint16_t node;
	std::istringstream ( substrs[0] ) >> node;

	PeripheralMessages::MessageId id;
	bool found = false;
	//convert enum message to number instead of the string for the mqtt string.
	for(size_t i = 0; i < sizeof(PeripheralMessages::MessageIdStrings); ++i) {
		if( strcmp(substrs[1].c_str(),PeripheralMessages::MessageIdStrings[i]) == 0) {
			id = static_cast<PeripheralMessages::MessageId>(i);
			found = true;
			break;
		}
	}
	//if the mqtt message name matched an id forward to the correct node
	if(found){
		send_rf24_message(node,id,substrs[2]);
	}
}

void MqttRf24Bridge::send_rf24_message(uint16_t node, PeripheralMessages::MessageId id, std::string& message_string)
{
	switch(id) {
	#define SWITCH_MESSAGE_CREATOR(MESSAGE) \
		case PeripheralMessages::MessageId::MESSAGE: \
		{ \
			PeripheralMessages::MESSAGE##Msg msg; \
			char *cstr = new char[message_string.length() + 1]; \
			strcpy(cstr, message_string.c_str()); \
			msg.set_message_from_mqtt_topic(cstr); \
			mRPIHandler.publish_message(msg,node); \
			delete[] cstr; \
			break; \
		}

			ALL_MESSAGES(SWITCH_MESSAGE_CREATOR)

	#undef SWITCH_MESSAGE_CREATOR
		default:
			break;
			//Unhandled messages dispose of since they are not used.
	}
}



std::vector<std::string> MqttRf24Bridge::split_mqtt_string(std::string &str)
{

	std::vector<std::string> mqtt_strs;
	std::size_t start = 0;
	std::size_t end = 0;

	//Pull out the Node ID and Message Id String
	for(int i = 0; i < 2; ++i)
	{
		end = str.find('/',start);
		if(end!=std::string::npos) {
			mqtt_strs.push_back(str.substr(start,end-start));
			start = end+1;
		}
		else {
			mqtt_strs.clear();
			return mqtt_strs;
		}
	}
	mqtt_strs.push_back(str.substr(start));
	return mqtt_strs;
}

/*std::stringstream ss(s);
	std::string item;
	std::vector<std::string> tokens;
	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}*/
