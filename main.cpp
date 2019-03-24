#include <stdio.h>
 #include <sys/time.h>
#include <sys/resource.h>
#include "mqtt_rfm69_bridge.hpp"


int main()
{	
	//This needs access to GPIO and SPI running at normal niceness 
	//has shown to lose packets over rf network on RPI.
	setpriority(PRIO_PROCESS, 0, -20);
	
	MqttRfm69Bridge bridge;

	while(1)
	{
		bridge.service_once();
	}

}


