#include "mosquittopp.h"
#include <functional>


class mqttwrapper : public mosqpp::mosquittopp
{
public:
	mqttwrapper(const char* id, const char* _topic, const char* host, int port,  bool sub=false);
	virtual ~mqttwrapper();
	bool send_message(const char * _message,const char* topic=NULL);
	void set_message_received_cb(std::function<void (const struct mosquitto_message *message)> callback);

private:
	const char* mHost;
	const char* mId;
	const char* mTopic;
	int mPort;
	int mKeepAlive;
	bool mSubscribe;
	volatile bool mPublished;
	void on_connect(int rc);
	void on_disconnect(int rc);
	void on_publish(int mid);
	void on_message(const struct mosquitto_message *message);
	std::function<void (const struct mosquitto_message *message)> mMessageReceivedCb;

};
