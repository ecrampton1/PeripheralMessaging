#ifndef MQTT_RF24_BRIDGE_HPP_
#define MQTT_RF24_BRIDGE_HPP_

#include "mqtt_wrapper.hpp"
#include "rf24_message_handler_rpi.hpp"
#include <memory>
#include <iostream>
#include <queue>

class MqttRf24Bridge
{
public:
	MqttRf24Bridge();

	/*@brief Call from main loop to service incoming mqtt messages*/
	void service_once();


private:

	/*@brief Handle outgoing RF messages by publishing them to the MQTT network.
	 * @details This templated function will handle all RF24 messages from
	 * @ref PeripheralMessages::MessageId.  All functions will be set as callbacks
	 * for the individual messages of the @ref PeripheralMessages::Message.
	 * @param args Callback arguments given should be the "this" pointer
	 * @param msg Received message cast to the templated type
	 * @param calling_id The node id of the rf24 node this is the static id set
	 * 		  at compile time of the individual node
	 */
	template<class T>
	static void handle_rf_message(void* args, void* msg, uint16_t calling_id)
	{
		MqttRf24Bridge& bridge = *(reinterpret_cast<MqttRf24Bridge*>(args));
		T& msg_obj = *(reinterpret_cast<T*>(msg));
		bridge.rf24_to_mqtt(msg_obj,calling_id);
	}

	template<class T>
	void rf24_to_mqtt(T& msg, uint16_t calling_id)
	{
		char buffer[64];
		//auto wrapper = std::unique_ptr<mqttwrapper>(new mqttwrapper(NULL,RF_TOPIC_OUT,MQTT_IP,MQTT_PORT));
		msg.get_message_as_mqtt_topic(buffer,sizeof(buffer));
		std::string mqtt_str(buffer);
		printf("%s\n",mqtt_str.c_str());
		std::ostringstream ostr;
		ostr << calling_id << "/" << msg.get_message_id_string() << "/" << mqtt_str;
		printf("%s\n",ostr.str().c_str());
		if(mMqttWrapper->send_message(ostr.str().c_str() ,RF_TOPIC_OUT) == false) {
			printf("Failed to send mqtt message: %s\n", ostr.str().c_str());
		}
		else {
			printf("Send Passed\n");
		}

	}

	void mqtt_to_rf24(std::string message);

	std::vector<std::string> split_mqtt_string(std::string &str);

	/*@brief Initialize the rf24 peripheral message handlers and mqtt subscription handler.*/
	void init();

	/*@brief handle incoming mqtt topics
	 * @param  message Mosquitto struct with mqtt topic information
	 */
	void handle_mqtt_topic(const struct mosquitto_message *message);

	void send_rf24_message(uint16_t node,PeripheralMessages::MessageId id, std::string& message_string);


	PeripheralMessages::RF24MessageHandlerRpi mRFRPIHandler;
	static constexpr const char* RF_GATEWAY = "RFGateway";
	static constexpr const char* RF_TOPIC_IN = "RFGateway/In";
	static constexpr const char* RF_TOPIC_OUT = "RFGateway/Out";
	static constexpr const char* MQTT_IP = "127.0.0.1";
	static constexpr int MQTT_PORT = 1883;

	std::queue<std::string> mIncomingTopics;
	std::unique_ptr<mqttwrapper> mMqttWrapper;

	PeripheralMessages::RF24MessageHandlerRpi mRPIHandler;

};

#endif //MQTT_RF24_BRIDGE_HPP_
