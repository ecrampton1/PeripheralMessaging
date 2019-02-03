#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_
#include "messaging.hpp"
#include <stdlib.h>


#ifndef EMBEDDED_MESSAGES
#include <string>
#include <sstream>      // std::ostringstream
#endif

using callback = void (*)(void*, void*,uint16_t);
namespace PeripheralMessages {

struct PayloadHeader
{
	MessageId mMessageId;
	uint8_t mNodeSensorId; //0-256 sensor id each sensor must have number to be used by handler
	uint8_t mReserved; /*reserved? maybe version*/
} __attribute__((packed));

struct MessageBuffer
{
	uint8_t* mBuffer;
	uint8_t mSize;
};

template<MessageId _messageId, class _messageStruct>
class Message
{
public:
	static constexpr uint8_t BUFFER_SIZE = sizeof(PayloadHeader) + sizeof(_messageStruct);

#ifndef EMBEDDED_MESSAGES
	Message(const std::string& mqtt_message) :
		Message()
	{

	}
#endif

	Message(uint8_t sensorId=0) :
	mMessageAllocated(true)
	{
		//Careful this should work now in MSP430, given there is implementation from the c library
		mMessageBuffer.mBuffer = (uint8_t*)malloc( BUFFER_SIZE );
		mMessageBuffer.mSize = BUFFER_SIZE;
		Initialize(mMessageBuffer.mBuffer,mMessageBuffer.mSize,true,sensorId);
	}  

	//Pass in already allocated buffer to be filled with message.
	Message(uint8_t* buffer, uint8_t bufferLength,uint8_t sensorId=0) :
	mMessageAllocated(false)
	{
		Initialize(buffer,bufferLength,true,sensorId);
	}

   //Creates new message if true
	Message(uint8_t* buffer, uint8_t bufferLength, bool createMessage,uint8_t sensorId=0) :
	mMessageAllocated(false)
	{
		mMessageAllocated = false;
		mMessageBuffer.mBuffer = buffer;
		mMessageBuffer.mSize = bufferLength;
		Initialize(buffer,bufferLength,createMessage,sensorId);
	}


	~Message()
	{

		if(mMessageAllocated) {
			//PRINT("dtor",ENDL)
			mMessageAllocated = false;
			//PRINT("free start",ENDL)
			free( (void*)mMessageBuffer.mBuffer );
			mMessageBuffer.mSize = 0;
			//PRINT("free end",ENDL)
		}
	}
	
	static void set_callback(callback cb)
	{
		mCallback = cb;
	}

	static void set_callback_arguments(void* ca)
	{
		mCallbackArgs = ca;
	}

	static void default_callback(void* args, void* msg, uint16_t callingId) {
		//Do nothing
	}
	
	static void trigger_callback(void* msg, uint16_t calling_id)  //add default callback that warns in compiler? or Assert to force it.
	{
		mCallback(mCallbackArgs, msg, calling_id);
	}

	const char* get_message_id_string() const
	{
		return MessageIdStrings[static_cast<uint16_t>(_messageId)];
	}

	const MessageBuffer&  get_message_buffer() const
	{
		return mMessageBuffer;
	}
	
	void set_sensor_id(const uint8_t sensorId)
	{
		mHeader->mNodeSensorId = sensorId;
	}


#ifndef EMBEDDED_MESSAGES

	void get_message_as_mqtt_topic(char* buffer, const size_t size)
	{

		mMessage->to_mqtt_string(buffer,size);
	}

#if 0
	static inline std::string message_to_mqtt(uint16_t msgId, SwitchMessage* const msg)
	{
		std::ostringstream ostr;
		ostr << MessageIdStrings[msgId] << "/" << (int)msg->switchNumber << "/" << msg->state;
		return ostr.str();
	}

	static inline std::string message_to_mqtt(uint16_t msgId, TemperatureMessage* const msg)
	{
		std::ostringstream ostr;
		ostr << MessageIdStrings[msgId] << "/" << msg->temperature;
		return ostr.str();
	}

	static inline std::string message_to_mqtt(uint16_t msgId, EmptyMessage* const msg)
	{
		std::ostringstream ostr;
		ostr << MessageIdStrings[msgId];
		return ostr.str();
	}

	static inline std::string message_to_mqtt(uint16_t msgId, VersionMessage* const msg)
	{
		std::ostringstream ostr;
		ostr << MessageIdStrings[msgId] << "/" << msg->major << "/" << msg->minor;
		return ostr.str();
	}

	static inline std::string message_to_mqtt(uint16_t msgId, PingPongMessage* const msg)
	{
		std::ostringstream ostr;
		ostr << MessageIdStrings[msgId] << "/" << msg->count;
		return ostr.str();
	}

	void set_message_from_mqtt(std::vector<std::string>& msg_sub_strings)
	{
		msg_sub_strings(mMessage);

	}
#endif
	void set_message_from_mqtt_topic(char* message)
	{
		mMessage->from_mqtt_string(message);
	}

#endif

	//Operator used when passing this message class as an argument
	//expecting a MessageBuffer struct, this is due to the MSP430 not
	//having the ability to use virtual and thus interface class.
	operator const MessageBuffer&() const { return get_message_buffer(); }


	PayloadHeader* const get_message_header() const
	{
		return mHeader;
	}


	//Be careful returns a pointer that can be modified by user
	//Will return a nullptr if things are not setup correctly check return
	_messageStruct* get_message_payload()
	{
		return mMessage;
	}
	

private:
	


	void Initialize(uint8_t* buffer, uint8_t bufferLength, bool createMessage, uint8_t sensorId)
	{
		if(bufferLength < sizeof(PayloadHeader) + sizeof(_messageStruct)) {
			mMessageBuffer.mBuffer = nullptr;
			mMessageBuffer.mSize = 0;
			mHeader = nullptr;
			mMessage = nullptr;
		}
		else {
			mHeader = reinterpret_cast<PayloadHeader*>(buffer);
			mMessage = reinterpret_cast<_messageStruct*>(buffer + sizeof(PayloadHeader));
			//After looking at this I dont know if I like this...Maybe fix this to be more clear due to addition of
			if(createMessage) {
				create_message(sensorId);
			}
		}

	}

	
	//Assumes pointers are correct no need to check again
	inline void create_message(uint8_t sensorId)
	{
		mHeader->mMessageId = _messageId;
		mHeader->mNodeSensorId = sensorId;
	}

	bool mMessageAllocated;
	MessageBuffer mMessageBuffer;
    PayloadHeader* mHeader;
	_messageStruct* mMessage;
	static callback mCallback;
	static void* mCallbackArgs;
};
template<MessageId m, class t>
callback Message<m,t>::mCallback = Message<m,t>::default_callback;
template<MessageId m, class t>
void* Message<m,t>::mCallbackArgs = nullptr;

using SwitchEventMsg  = Message<MessageId::SwitchEvent,SwitchMessage>;
using SwitchDataMsg  = Message<MessageId::SwitchData,SwitchMessage>;
using SwitchQueryMsg = Message<MessageId::SwitchQuery,SwitchMessage>;
using SwitchRequestMsg = Message<MessageId::SwitchRequest,SwitchMessage>;

using TemperatureEventMsg  = Message<MessageId::TemperatureEvent,TemperatureMessage>;
using TemperatureDataMsg  = Message<MessageId::TemperatureData,TemperatureMessage>;
using TemperatureQueryMsg = Message<MessageId::TemperatureQuery,EmptyMessage>;
using TemperatureRequestMsg = Message<MessageId::TemperatureRequest,EmptyMessage>;

using VersionDataMsg  = Message<MessageId::VersionData,VersionMessage>;
using VersionQueryMsg = Message<MessageId::VersionQuery,EmptyMessage>;

using PingPongDataMsg  = Message<MessageId::PingPongData,PingPongMessage>;
using PingPongQueryMsg = Message<MessageId::PingPongQuery,PingPongMessage>;
	

}
#endif //MESSAGE_HPP_

