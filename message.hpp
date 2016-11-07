#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_
#include "messaging.hpp"
//#include "mcu_config.hpp"
#include <stdlib.h>

using callback = void (*)(void*, void*,uint16_t);
namespace PeripheralMessages {

struct PayloadHeader
{
	MessageId mMessageId;
	uint16_t mReserved; /*reserved? maybe version*/
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
	Message() :
	mMessageAllocated(true)
	{
		//Careful this should work now in MSP430, given there is implementation from the c library
		mMessageBuffer.mBuffer = (uint8_t*)malloc( BUFFER_SIZE );
		mMessageBuffer.mSize = BUFFER_SIZE;
		Initialize(mMessageBuffer.mBuffer,mMessageBuffer.mSize,true);
	}  

	//Pass in already allocated buffer to be filled with message.
	Message(uint8_t* buffer, uint8_t bufferLength) :
	mMessageAllocated(false)
	{
		Initialize(buffer,bufferLength,true);
	}

   //Creates new message if true
	Message(uint8_t* buffer, uint8_t bufferLength, bool createMessage) :
	mMessageAllocated(false)
	{
		mMessageAllocated = false;
		mMessageBuffer.mBuffer = buffer;
		mMessageBuffer.mSize = bufferLength;
		Initialize(buffer,bufferLength,createMessage);
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

	static void default_callback(void* args, void* msg, uint16_t calling_id) {
		//Do nothing
	}
	
	static void trigger_callback(void* msg, uint16_t calling_id)  //add default callback that warns in compiler? or Assert to force it.
	{
		mCallback(mCallbackArgs, msg, calling_id);
	}


	const MessageBuffer&  get_message_buffer() const
	{
		return mMessageBuffer;
	}
	
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
	
	void Initialize(uint8_t* buffer, uint8_t bufferLength, bool createMessage)
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
			if(createMessage) {
				create_message();
			}
		}
	}

	
	//Assumes pointers are correct no need to check again
	inline void create_message()
	{
		mHeader->mMessageId = _messageId;
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

using TemperatureEventMsg  = Message<MessageId::SwitchEvent,TemperatureMessage>;
using TemperatureDataMsg  = Message<MessageId::SwitchData,TemperatureMessage>;
using TemperatureQueryMsg = Message<MessageId::SwitchQuery,EmptyMessage>;
using TemperatureRequestMsg = Message<MessageId::SwitchRequest,EmptyMessage>;

using VersionDataMsg  = Message<MessageId::VersionData,VersionMessage>;
using VersionQueryMsg = Message<MessageId::VersionQuery,EmptyMessage>;

using PingPongDataMsg  = Message<MessageId::PingPongData,PingPongMessage>;
using PingPongQueryMsg = Message<MessageId::PingPongQuery,PingPongMessage>;
	

}
#endif //MESSAGE_HPP_

