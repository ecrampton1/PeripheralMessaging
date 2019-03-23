#include "mqtt_rfm69_bridge.hpp"
#include <string.h>

using Handler = PeripheralMessages::RFM69Handler;

MqttRfm69Bridge::MqttRfm69Bridge()
{
	Handler::begin(10);
	init();
	
}


void MqttRfm69Bridge::handle_mqtt_topic(const struct mosquitto_message *message)
{
	//printf("%s %s\n",message->topic,(const char*)message->payload);
	string_double strs = std::make_tuple (std::string((const char*)message->topic),std::string((const char*)message->payload));
	mIncomingTopics.push(strs);
}



void MqttRfm69Bridge::init()
{
#define INIT_MSG_HANDLERS(MSG) \
	PeripheralMessages::MSG##Msg::set_callback(&MqttRfm69Bridge::handle_rf_message<PeripheralMessages::MSG##Msg>); \
	PeripheralMessages::MSG##Msg::set_callback_arguments(this);
	ALL_MESSAGES(INIT_MSG_HANDLERS)
#undef INIT_HANDLERS
	try {
		mMqttWrapper = std::unique_ptr<mqttwrapper>(new mqttwrapper(NULL,"#",MQTT_IP,MQTT_PORT,true));
		mMqttWrapper->set_message_received_cb(std::bind(&MqttRfm69Bridge::handle_mqtt_topic,this, std::placeholders::_1));
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}


void MqttRfm69Bridge::service_once()
{
	mMqttWrapper->service_once();

	//handle incoming topics
	if(!mIncomingTopics.empty()) {
		auto topic = mIncomingTopics.front();
		mqtt_to_rfm69(topic);
		mIncomingTopics.pop();
	}
	//handle incoming rf network messages
	Handler::serviceOnce();
}

void MqttRfm69Bridge::mqtt_to_rfm69(string_double strs)
{
	std::vector<std::string> sub_strs = split_string(std::get<0>(strs));
	
	if(false == checkIncomingTopic(sub_strs)) {
	
		return;
	}	
	
	uint16_t node;
	std::istringstream ( sub_strs[2] ) >> node;

	int16_t sensor_id;
	std::istringstream ( sub_strs[3] ) >> sensor_id;

	PeripheralMessages::MessageId msg_id;
	bool found = false;
	
	//convert enum message to number instead of the string for the mqtt string.
	for(size_t i = 0; i < static_cast<uint16_t>(PeripheralMessages::MessageId::MESSAGE_EOL); ++i) {
		if( strcmp(sub_strs[4].c_str(),PeripheralMessages::MessageIdStrings[i]) == 0) {
			msg_id = static_cast<PeripheralMessages::MessageId>(i);
			found = true;
			break;
		}
	}
	//if the mqtt message name matched an id forward to the correct node
	if(found){
		send_rfm69_message(node, sensor_id, msg_id,std::get<1>(strs).c_str());
	}
}

bool MqttRfm69Bridge::checkIncomingTopic(const std::vector<std::string>& topic_strings)
{
	if(topic_strings.size() < 2) {
		printf("Not enough strings to build packet\n");
		return false; //too small of a string to have any information
	}
	
	if( strcmp(topic_strings[0].c_str(),RF_GATEWAY) != 0) {
		return false;
	}
	
	if( strcmp(topic_strings[1].c_str(),RF_TOPIC_RESET) == 0) {
		Handler::begin(10);
		return false;
	}
	
	if( strcmp(topic_strings[1].c_str(),RF_TOPIC_IN) == 0 &&
		topic_strings.size() > TOPIC_MIN_LENGTH ) {
		return true;
	}
	return false;
}

void MqttRfm69Bridge::send_rfm69_message(uint16_t node, uint8_t sensorId, PeripheralMessages::MessageId msgId, std::string messageString)
{
	switch(msgId) {
	#define SWITCH_MESSAGE_CREATOR(MESSAGE) \
		case PeripheralMessages::MessageId::MESSAGE: \
		{ \
			PeripheralMessages::MESSAGE##Msg msg(sensorId); \
			char *cstr = new char[messageString.length() + 1]; \
			strcpy(cstr, messageString.c_str()); \
			msg.set_message_from_mqtt_topic(cstr); \
			Handler::publishMessage(msg,node); \
			delete[] cstr; \
			break; \
		}

			ALL_MESSAGES(SWITCH_MESSAGE_CREATOR)

	#undef SWITCH_MESSAGE_CREATOR
		default:
		printf("Default: %d\n", msgId);
			break;
			//Unhandled messages dispose of since they are not used.
	}
}



std::vector<std::string> MqttRfm69Bridge::split_string(std::string str)
{
	std::vector<std::string> sub_strs;
	char delimiter = '/';

	std::stringstream ss(str);
	std::string item;
	std::vector<std::string> tokens;
	while (getline(ss, item, delimiter)) {
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
