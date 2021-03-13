#ifndef MQTT_RFM69_BRIDGE_HPP_
#define MQTT_RFM69_BRIDGE_HPP_

#include "mqtt_wrapper.hpp"
#include "rfm69_message_handler.hpp"
#include <memory>
#include <iostream>
#include <queue>
#include <tuple>
#include <vector>
#include <map>
#include <chrono>


struct NodeInfo
{
	uint8_t mNodeNumber;
	std::chrono::time_point<std::chrono::system_clock> mLastTimeStamp;
	std::vector<PeripheralMessages::ObjectType> mNodeObjects;

	static std::string object_to_component(PeripheralMessages::ObjectType type)
	{
		std::string s;

		switch(type)
		{
		case PeripheralMessages::ObjectType::SWITCH_CONTROL_OBJECT:
			s = "switch";
			break;
		//SHould also include any other sensor....
		case PeripheralMessages::ObjectType::TEMPERATURE_SENSOR_OBJECT:
			s = "sensor";
			break;
		default:
			//Do what?
			break;
		}
		return s;
	}

	static std::string object_to_config(const PeripheralMessages::ObjectType type, const uint16_t node, const int object)
	{
		std::string s;
		std::string objectNameStr;
		std::string nodeSensorStr = std::to_string(node) + "/" + std::to_string(object);

		switch(type)
		{
		case PeripheralMessages::ObjectType::SWITCH_CONTROL_OBJECT:
			objectNameStr = "switch_" + std::to_string(node) + "_" + std::to_string(object);
			s = "{\"name\": \"" + objectNameStr + "\", \"command_topic\": \"RFGateway/In/switch/" + nodeSensorStr +
			"\", \"state_topic\": \"RFGateway/Out/switch/" + nodeSensorStr + "\"}";
			break;
		//SHould also include any other sensor....
		case PeripheralMessages::ObjectType::TEMPERATURE_SENSOR_OBJECT:
			objectNameStr = "temp_" + std::to_string(node) + "_" + std::to_string(object);
			s = "{\"name\": \"" + objectNameStr + "\", \"state_topic\": \"RFGateway/Out/sensor/" + nodeSensorStr + "\"}";
			break;
		default:
			//Do what?
			break;
		}
		return s;
	}
};

class MqttRfm69Bridge
{
public:
	friend class PrivateHandler;
	MqttRfm69Bridge();

	/*@brief Call from main loop to service incoming mqtt messages*/
	void service_once();


private:

	//Hold all the information on the nodes
	std::map<uint8_t,NodeInfo> mNodeDatabase;

	using string_double = std::tuple<std::string,std::string>;

	/*@brief Handle outgoing RF messages by publishing them to the MQTT network.
	 * @details This templated function will handle all RF24 messages from
	 * @ref PeripheralMessages::MessageId.  All functions will be set as callbacks
	 * for the individual messages of the @ref PeripheralMessages::Message.
	 * @param args Callback arguments given should be the "this" pointer
	 * @param msg Received message cast to the templated type
	 * @param calling_id The node id of the rfm69 node this is the static id set
	 * 		  at compile time of the individual node
	 */
	template<class T>
	static void handle_rf_message(void* args, void* msg, uint16_t callingId);

	void handle_init_query(void* args, void* msg, uint16_t callingId);

	uint8_t add_to_database();

	void update_database(uint16_t callingId);


	template<class T>
	void rfm69_to_mqtt(T& msg, uint16_t callingId)
	{
		char buffer[64];
		//auto wrapper = std::unique_ptr<mqttwrapper>(new mqttwrapper(NULL,RF_TOPIC_OUT,MQTT_IP,MQTT_PORT));
		msg.get_message_as_mqtt_topic(buffer,sizeof(buffer));
		std::string mqtt_str(buffer);
		std::ostringstream ostr;
		ostr << RF_GATEWAY << "/" << RF_TOPIC_OUT << "/" << callingId << "/" << (int)msg.get_message_header()->mNodeSensorId << "/" << msg.get_message_id_string();
		//printf("%s\n",ostr.str().c_str());
		if(mMqttWrapper->send_message(mqtt_str.c_str() ,ostr.str().c_str()) == false) {
			printf("Failed to send mqtt message: %s\n", ostr.str().c_str());
		}

	}

	bool checkIncomingTopic(const std::vector<std::string>& topic_strings);

	void mqtt_to_rfm69(string_double message);

	std::vector<std::string> split_string(std::string str);

	/*@brief Initialize the rfm69 peripheral message handlers and mqtt subscription handler.*/
	void init();


	/*@brief handle incoming mqtt topics
	 * @param  message Mosquitto struct with mqtt topic information
	 */
	void handle_mqtt_topic(const struct mosquitto_message *message);

	void send_rfm69_message(uint16_t node, uint8_t sensorId,PeripheralMessages::MessageId msgId, std::string messageString);


	static constexpr const char* RF_GATEWAY = "RFGateway";
	static constexpr const char* RF_TOPIC_RESET = "Reset";
	static constexpr const char* RF_TOPIC_IN = "In";
	static constexpr const char* RF_TOPIC_OUT = "Out";
	static constexpr const char* RF_TOPIC_IN_CAT = "RFGateway/In";
	static constexpr int TOPIC_MIN_LENGTH = 4;
	static constexpr const char* MQTT_IP = "127.0.0.1";
	static constexpr int MQTT_PORT = 1883;

	std::queue<string_double > mIncomingTopics;
	std::unique_ptr<mqttwrapper> mMqttWrapper;

	//PeripheralMessages::RFM69Handler mHandler;

};

#endif //MQTT_RFM69_BRIDGE_HPP_
