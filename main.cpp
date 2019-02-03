#include <stdio.h>
//#include "mqtt_rf24_bridge.hpp"
#include "rfm69_message_handler.hpp"
#include "sys_wrapper.hpp"


using Handler = PeripheralMessages::RFM69Handler;

uint8_t PongerAddress = 9;
uint8_t PingerAddress = 10;

static uint32_t TimeSinceSent = 0;
static PeripheralMessages::PingPongQueryMsg ping;

void handle_pong(void* args, void*  msg, uint16_t calling_id)
{
	PeripheralMessages::PingPongDataMsg* pong_ptr =
			static_cast<PeripheralMessages::PingPongDataMsg*>(msg);
	printf("Pong received: %u, Node: %d \n", pong_ptr->get_message_payload()->count, calling_id);

	
	++ping.get_message_payload()->count;

	Handler::publish_message(ping,calling_id);
	TimeSinceSent = SysWrapper::millis();
}

#if 0
PeripheralMessages::RF24MessageHandlerRpi rpihandler;
const std::string RF_TOPIC_IN = "RFGatewayIn";
const std::string RF_TOPIC_OUT = "RFGatewayOut";
const std::string MQTT_IP = "127.0.0.1";
const int MQTT_PORT = 1883;

std::queue<std::string> incoming_topics;


/*void mqtt_callback(const struct mosquitto_message *message)
{
	std::cout << message->topic << std::endl;
}*/

template<class T>
void handle_message(void* args, void* msg, uint16_t calling_id)
{
	T& obj = *(reinterpret_cast<T*>(msg));

}

void handle_mqtt_topic(const struct mosquitto_message *message)
{
	if(strcmp(message->topic,RF_TOPIC_IN.c_str()) == 0) {
		incoming_topics.push((const char*)message->payload);
		printf("message: %s\n", (const char*)message->payload);

	}
}

std::unique_ptr<mqttwrapper> mqtt_topic_sub;

void init()
{
#define INIT_MSG_HANDLERS(MSG) \
	PeripheralMessages::MSG##Msg::set_callback(&handle_message<PeripheralMessages::MSG##Msg>);

	ALL_MESSAGES(INIT_MSG_HANDLERS)
#undef INIT_HANDLERS
	try {
		mqtt_topic_sub = std::unique_ptr<mqttwrapper>(new mqttwrapper(NULL,RF_TOPIC_IN.c_str(),MQTT_IP.c_str(),MQTT_PORT,true));
		mqtt_topic_sub->set_message_received_cb(&handle_mqtt_topic);
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}
#endif
static uint64_t TIMEOUT = 50;
int main()
{
	/*PeripheralMessages::SwitchRequestMsg msg;
	msg.get_message_payload()->switchNumber = 0;
	msg.get_message_payload()->state = true;
	printf("%s\n", msg.get_message_as_mqtt_topic().c_str());*/
	Handler::begin(PingerAddress);
	PeripheralMessages::PingPongDataMsg::set_callback(&handle_pong);
	//MqttRf24Bridge bridge;
	
	Handler::publish_message(ping,PongerAddress);
	TimeSinceSent = SysWrapper::millis();
	printf("Setup\n");
	while(1)
	{
		Handler::serviceOnce();
		if((TimeSinceSent + TIMEOUT) < SysWrapper::millis()){
			printf(".\n");
			Handler::publish_message(ping,PongerAddress);
			TimeSinceSent = SysWrapper::millis();
		}
	}

}


