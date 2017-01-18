#include "mqtt_rf24_bridge.hpp"
#include <string.h>


MqttRf24Bridge::MqttRf24Bridge()
{
	init();
}


void MqttRf24Bridge::handle_mqtt_topic(const struct mosquitto_message *message)
{
	printf("%s %s\n",message->topic,(const char*)message->payload);
	if(strncmp(message->topic,RF_TOPIC_IN,sizeof(RF_TOPIC_IN)) == 0) {
		string_double strs = std::make_tuple (std::string((const char*)message->topic),std::string((const char*)message->payload));
		mIncomingTopics.push(strs);
	}
	printf("%u",mIncomingTopics.empty());
}



void MqttRf24Bridge::init()
{
#define INIT_MSG_HANDLERS(MSG) \
	PeripheralMessages::MSG##Msg::set_callback(&MqttRf24Bridge::handle_rf_message<PeripheralMessages::MSG##Msg>); \
	PeripheralMessages::MSG##Msg::set_callback_arguments(this);
	ALL_MESSAGES(INIT_MSG_HANDLERS)
#undef INIT_HANDLERS
	try {
		mMqttWrapper = std::unique_ptr<mqttwrapper>(new mqttwrapper(NULL,"#",MQTT_IP,MQTT_PORT,true));
		mMqttWrapper->set_message_received_cb(std::bind(&MqttRf24Bridge::handle_mqtt_topic,this, std::placeholders::_1));
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}

void MqttRf24Bridge::service_once()
{
	mMqttWrapper->service_once();

	//handle incoming topics
	if(!mIncomingTopics.empty()) {
		auto topic = mIncomingTopics.front();
		mqtt_to_rf24(topic);
		mIncomingTopics.pop();
	}
	//handle incoming rf network messages
	mRPIHandler.service_once();
}

void MqttRf24Bridge::mqtt_to_rf24(string_double strs)
{

	std::vector<std::string> substrs = split_string(std::get<0>(strs));

	for (auto i = substrs.begin(); i != substrs.end(); ++i)
	    std::cout << *i << ' ';
	std::cout << std::endl;
	if(substrs.size() < 1) {
		return; //too small of a string to have any information
	}
	uint16_t node;
	std::istringstream ( substrs[2] ) >> node;

	int16_t sensorId;
	std::istringstream ( substrs[3] ) >> sensorId;


	PeripheralMessages::MessageId msgId;
	bool found = false;
	//convert enum message to number instead of the string for the mqtt string.
	for(size_t i = 0; i < sizeof(PeripheralMessages::MessageIdStrings); ++i) {
		if( strcmp(substrs[4].c_str(),PeripheralMessages::MessageIdStrings[i]) == 0) {
			printf("%d\n",i);
			msgId = static_cast<PeripheralMessages::MessageId>(i);
			found = true;
			break;
		}
	}
	//if the mqtt message name matched an id forward to the correct node
	if(found){
		send_rf24_message(node, sensorId, msgId,std::get<1>(strs).c_str());
	}
}

void MqttRf24Bridge::send_rf24_message(uint16_t node, uint8_t sensorId, PeripheralMessages::MessageId msgId, std::string message_string)
{
	switch(msgId) {
	#define SWITCH_MESSAGE_CREATOR(MESSAGE) \
		case PeripheralMessages::MessageId::MESSAGE: \
		{ \
			PeripheralMessages::MESSAGE##Msg msg(sensorId); \
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



std::vector<std::string> MqttRf24Bridge::split_string(std::string str)
{
	std::cout << str << std::endl;
	std::vector<std::string> sub_strs;
	char delimiter = '/';

	std::stringstream ss(str);
	std::string item;
	std::vector<std::string> tokens;
	while (getline(ss, item, delimiter)) {
		std::cout << item << std::endl;
		sub_strs.push_back(item);
	}

	return sub_strs;
}



/*std::stringstream ss(s);
	std::string item;
	std::vector<std::string> tokens;
	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}*/
