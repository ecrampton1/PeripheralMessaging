#include <stdio.h>
#include "mqtt_rfm69_bridge.hpp"


int main()
{	
	MqttRfm69Bridge bridge;

	while(1)
	{
		bridge.service_once();
	}

}


