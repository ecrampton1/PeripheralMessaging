

#include <iostream>
#include "mqtt_wrapper.hpp"
#include <string.h>
#include <time.h>
#include <unistd.h>

mqttwrapper::mqttwrapper(const char* id,const char*  topic, const char* host, int port, bool sub) :
mosqpp::mosquittopp(id),
mHost(host),
mId(id),
mTopic(topic),
mPort(port),
mKeepAlive(60),
mSubscribe(sub),
mPublished(false)
{
	if(sub) {
		mKeepAlive = 0;
	}
	mosqpp::lib_init();        // Mandatory initialization for mosquitto library

	connect(mHost,     // non blocking connection to broker request
	mPort,
	mKeepAlive);
	loop(0,1);            // Start mqtt
};

mqttwrapper::~mqttwrapper() {
 loop_stop();            // Kill the thread
 mosqpp::lib_cleanup();    // Mosquitto library cleanup
}


bool mqttwrapper::send_message(const  char * message, const char* topic)
{
	if(topic == NULL) {
		topic = mTopic;
	}
	mPublished = false;
	// Send message - depending on QoS, mosquitto lib managed re-submission this the thread
	//
	// * NULL : Message Id (int *) this allow to latter get status of each message
	// * topic : topic to be used
	// * lenght of the message
	// * message
	// * qos (0,1,2)
	// * retain (boolean) - indicates if message is retained on broker or not
	// Should return MOSQ_ERR_SUCCESS
	int ret = publish(NULL,topic,strlen(message),message,0,false);
	if( ret != MOSQ_ERR_SUCCESS ) {
		printf("ret: %d\n", ret);
		return false;
	}
	time_t end = time (NULL) + 15;
	while(mPublished == false); //{ if(time(NULL) > end) return false; }
	return mPublished;
}

void mqttwrapper::service_once()
{
	int ret = loop(0,1);            //service mqtt
	if(ret != MOSQ_ERR_SUCCESS) {
		printf("Error mqtt connection lost\n");
		sleep(10);
		reconnect();
	}
}

void mqttwrapper::on_disconnect(int rc) {
 std::cout << ">> myMosq - disconnection(" << rc << ")" << std::endl;
 }

void mqttwrapper::on_connect(int rc)
{
	if ( rc == 0 ) {
		std::cout << ">> myMosq - connected with server" << std::endl;
		if(mSubscribe)
			subscribe(NULL,mTopic,0);
	} else {
		std::cout << ">> myMosq - Impossible to connect with server(" << rc << ")" << std::endl;
	}
}

void mqttwrapper::set_message_received_cb(std::function<void (const struct mosquitto_message *message)> callback)
{
	mMessageReceivedCb = callback;
}

void mqttwrapper::on_message(const struct mosquitto_message *message)
{
	mMessageReceivedCb(message);
}

void mqttwrapper::on_publish(int mid)
{
	mPublished = true;
}

