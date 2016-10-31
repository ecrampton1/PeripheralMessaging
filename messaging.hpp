#ifndef MESSAGING_HPP_
#define MESSAGING_HPP_
#include <stdint.h>
#include <stddef.h>

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
	MESSAGE_ID_DQ(PingPong,ACTION)


namespace PeripheralMessages {

enum class MessageId : uint16_t
{
	ALL_MESSAGES(COMMA)
};

struct EmptyMessage
{
	uint8_t reserved;
};


struct PingPongMessage
{
	uint32_t count;
};


struct VersionMessage
{
	uint8_t major;
	uint8_t minor;
	uint8_t reserved;
};

struct SwitchMessage
{
	uint8_t switchNumber; //Which switch Up to 256
	bool    state;  //Whats the state True is on False for off
};

struct TemperatureMessage
{
	int16_t temperature;
};

}






#endif

