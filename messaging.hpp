#ifndef MESSAGING_HPP_
#define MESSAGING_HPP_
#include <stdint.h>
#include <stddef.h>

//TODO automate this in the build process
#define VERSION_MAJOR 0
#define VERSION_MINOR 1


//Data sent out from a query
//Query just asks for the data
#define MESSAGE_ID_DQ(MSG) \
	MSG##Query, \
	MSG##Data

//Event is something triggered that is sent out
//Request is requesting the sensor to do something
#define MESSAGE_ID_ER(MSG) \
	MSG##Request, \
	MSG##Event 

//In some cases a message set will have all types of messages.
#define MESSAGE_ID_ALL(MSG) \
	MESSAGE_ID_DQ(MSG), \
	MESSAGE_ID_ER(MSG) 

#define COMMA(x) x,

#define ALL_MESSAGES(ACTION) \
	ACTION(MESSAGE_ID_DQ(Version)) \
	ACTION(MESSAGE_ID_ALL(Switch)) \
	MESSAGE_ID_ALL(Temperature)


namespace PeripheralMessages {

enum class MessageId : uint16_t
{
	ALL_MESSAGES(COMMA)
};

struct EmptyMessage
{
	uint8_t reserved;
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

