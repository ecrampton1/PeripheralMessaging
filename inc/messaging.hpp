#ifndef MESSAGING_HPP_
#define MESSAGING_HPP_
#include <stdint.h>
#include <stddef.h>
#include "mqtt_helper.hpp"

//TODO automate this in the build process
#define VERSION_MAJOR 0
#define VERSION_MINOR 1


//Data sent out from a query
//Query just asks for the data
#define MESSAGE_ID_DQ(MSG,ACTION) \
	ACTION(MSG##Query) \
	ACTION(MSG##Data)

//Event is something triggered that is sent out
//Request is requesting the sensor to do something
#define MESSAGE_ID_ER(MSG,ACTION) \
	ACTION(MSG##Request) \
	ACTION(MSG##Event)

//In some cases a message set will have all types of messages.
#define MESSAGE_ID_ALL(MSG,ACTION) \
	MESSAGE_ID_DQ(MSG,ACTION) \
	MESSAGE_ID_ER(MSG,ACTION)

#define COMMA(x) x,

#define ALL_MESSAGES(ACTION) \
	MESSAGE_ID_DQ(Version,ACTION) \
	MESSAGE_ID_ALL(Switch,ACTION) \
	MESSAGE_ID_ALL(Temperature,ACTION) \
	MESSAGE_ID_DQ(PingPong,ACTION) \
	MESSAGE_ID_DQ(Heartbeat,ACTION) \
	MESSAGE_ID_DQ(DistanceSensor,ACTION) \
	MESSAGE_ID_DQ(NodeInit,ACTION) \
	MESSAGE_ID_DQ(Objects,ACTION) \

#define MESSAGE_ID_TO_STRING(MSG) #MSG,


namespace PeripheralMessages {

enum class MessageId : uint16_t
{
	ALL_MESSAGES(COMMA)
	MESSAGE_EOL	
};

static const char* MessageIdStrings[] = {
		ALL_MESSAGES(MESSAGE_ID_TO_STRING)
};

//Object type refers to the type of controller/sensor that is attached
//to this node
enum class ObjectType : uint8_t
{
	SWITCH_CONTROL_OBJECT=0,
	LIGHT_CONTROL_OBJECT,
	TEMPERATURE_SENSOR_OBJECT,
	HUMIDITY_SENSOR_OBJECT,
	PRESURE_SENSOR_OBJECT,
	COVER_CONTROL_OBJECT,
	LOCK_CONTROL_OBJECT,
	DISTANCE_SENSOR_OBJECT,
	SERVO_CONTROL_OBJECT,
	POWER_SENSOR_OBJECT,
	WEIGHT_SENSOR_OBJECT,
	MOTION_SENSOR_OBJECT
};

//const uint16_t MAX_MESSAGE_IDS = sizeof(MessageIdStrings);


#define STRUCT_NAME EmptyMessage
#define STRUCT_FIELDS \
	X(uint8_t, reserved)
#include "message_macro_gen.hpp"


//Used to return a new node address
#define STRUCT_NAME NodeInitMessage
#define STRUCT_FIELDS \
	X(uint8_t, id)
#include "message_macro_gen.hpp"

//Objects (sensors,switchs,etc) on this node upto 20 per node
#define STRUCT_NAME ObjectsMessage
#define STRUCT_FIELDS \
	X(uint8_t,numOfObjects)
#define STRUCT_ARRAY_FIELDS \
	X(uint8_t,objects,16)
#include "message_macro_gen.hpp"

#define STRUCT_NAME PingPongMessage
#define STRUCT_FIELDS \
	X(uint32_t, count)
#include "message_macro_gen.hpp"

#define STRUCT_NAME HeartbeatMessage
#define STRUCT_FIELDS \
	X(uint32_t, upTimeInSeconds)
#include "message_macro_gen.hpp"


#define STRUCT_NAME VersionMessage
#define STRUCT_FIELDS \
	X(uint8_t, major) \
	X(uint8_t, minor) \
	X(uint16_t, reserved)
#include "message_macro_gen.hpp"


#define STRUCT_NAME SwitchMessage
#define STRUCT_FIELDS \
	X(int8_t,   state )
#include "message_macro_gen.hpp"


#define STRUCT_NAME TemperatureMessage
#define STRUCT_FIELDS \
	X(int16_t, temperature)
#include "message_macro_gen.hpp"

#define STRUCT_NAME DistanceSensorMessage
#define STRUCT_FIELDS \
	X(uint16_t, distanceInCm)
#include "message_macro_gen.hpp"

}






#endif

