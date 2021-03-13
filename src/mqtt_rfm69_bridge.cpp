#include "mqtt_rfm69_bridge.hpp"
#include <string.h>

using Handler = PeripheralMessages::RFM69Handler;



MqttRfm69Bridge::MqttRfm69Bridge()
{
	//Use default values.
	Handler::begin(Handler::GATEWAY_ID,Handler::NETWORK_ADDRESS);
	init();
}


void MqttRfm69Bridge::handle_mqtt_topic(const struct mosquitto_message *message)
{
	//printf("%s %s\n",message->topic,(const char*)message->payload);
	string_double strs = std::make_tuple (std::string((const char*)message->topic),std::string((const char*)message->payload));
	mIncomingTopics.push(strs);
}

template<>
void MqttRfm69Bridge::handle_rf_message<PeripheralMessages::NodeInitQueryMsg>(void* args, void* msg, uint16_t callingId)
{
	MqttRfm69Bridge& bridge = *(reinterpret_cast<MqttRfm69Bridge*>(args));
	PeripheralMessages::NodeInitQueryMsg& msg_obj = *(reinterpret_cast<PeripheralMessages::NodeInitQueryMsg*>(msg));

	PeripheralMessages::NodeInitDataMsg nodeData;
	nodeData.get_message_payload()->id = bridge.add_to_database();
	std::cout << "New Node: " << (int)nodeData.get_message_payload()->id << std::endl;

	Handler::publishMessage(nodeData,callingId);
}

template<>
void MqttRfm69Bridge::handle_rf_message<PeripheralMessages::ObjectsDataMsg>(void* args, void* msg, uint16_t callingId)
{
	MqttRfm69Bridge& bridge = *(reinterpret_cast<MqttRfm69Bridge*>(args));
	PeripheralMessages::ObjectsDataMsg& msg_obj = *(reinterpret_cast<PeripheralMessages::ObjectsDataMsg*>(msg));
	uint8_t num = (msg_obj.get_message_payload()->numOfObjects > sizeof(PeripheralMessages::ObjectsMessage::objects)) ?
					0 : msg_obj.get_message_payload()->numOfObjects;


	if(num > 0)
	{
		auto it = bridge.mNodeDatabase.find(callingId);
		if(it != bridge.mNodeDatabase.end())
		{
			//empty current set of objects
			it->second.mNodeObjects.clear();
		}
		else
		{
			//initialize the calling id in the case it is not saved in our database.
			NodeInfo node{callingId,std::chrono::system_clock::now()};

			bridge.mNodeDatabase[callingId] = node;
		}

		for(uint8_t i = 0; i < msg_obj.get_message_payload()->numOfObjects; ++i)
		{
			bridge.mNodeDatabase[callingId].mNodeObjects.push_back(static_cast<PeripheralMessages::ObjectType>(msg_obj.get_message_payload()->objects[i]));
		}

		int n = 0;

		for(auto obj : bridge.mNodeDatabase[callingId].mNodeObjects)
		{
			std::ostringstream topic_str;
			std::string nodeSensorStr = std::to_string(callingId) + "/" + std::to_string(n);
			topic_str << "homeassistant" << "/" << NodeInfo::object_to_component(obj) << "/"
					<< nodeSensorStr << "/" << "config";
			std::string config_str = NodeInfo::object_to_config(obj,callingId,n);

			std::cout << topic_str.str() << " -m " << NodeInfo::object_to_config(obj,callingId,n) << std::endl;
			//below is an example.  need to create the message that is sent with the config to add the command topic and state topic.
			//<discovery_prefix>/<component>/[<node_id>/]<object_id>/config -m '{"name": "garden", "command_topic": "homeassistant/switch/irrigation/set", "state_topic": "homeassistant/switch/irrigation/state"}'

			if(bridge.mMqttWrapper->send_message( config_str.c_str(),topic_str.str().c_str()) == false) {
				printf("Failed to send mqtt message: %s\n", config_str.c_str());
			}
			topic_str.clear();
			n++;
		}


	}
}



uint8_t MqttRfm69Bridge::add_to_database()
{
	uint8_t prev = 1;
	uint8_t id = 2;//skip the first 2, reserved gateways
	std::chrono::system_clock::now();

	for(auto node : mNodeDatabase)
	{
		if((node.first - prev) > 1)
		{
			id = prev+1;
		}
	}
	NodeInfo node{id,std::chrono::system_clock::now()};

	mNodeDatabase[id] = node;
	return id;
}

template<class T>
void MqttRfm69Bridge::handle_rf_message(void* args, void* msg, uint16_t callingId)
{
	MqttRfm69Bridge& bridge = *(reinterpret_cast<MqttRfm69Bridge*>(args));
	T& msg_obj = *(reinterpret_cast<T*>(msg));
	bridge.rfm69_to_mqtt(msg_obj,callingId);
	bridge.update_database(callingId);
}

void MqttRfm69Bridge::update_database(uint16_t callingId)
{
	auto it = mNodeDatabase.find(callingId);
	if(it != mNodeDatabase.end()) {
		std::cout << "DB" << callingId << std::endl;
		mNodeDatabase[callingId].mLastTimeStamp = std::chrono::system_clock::now();
	}
	else
	{
		std::cout << "Request Object Query " << callingId << std::endl;
		//request the objects
		PeripheralMessages::ObjectsQueryMsg msg;
		Handler::publishMessage(msg,callingId);
	}
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

	//initialize the node array with first 2 reserved for gateway use
	for(int i = 0 ; i < 2; ++i)//add define for number of gateway ids on a network?
	{
		//Used in future for multiple gateways?
		NodeInfo node{i,std::chrono::system_clock::now()};
		mNodeDatabase[i] = node;
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
		Handler::begin(Handler::GATEWAY_ID,Handler::NETWORK_ADDRESS);
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
