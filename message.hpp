#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_
#include "messaging.hpp"


using callback = void (*)(void*,uint16_t);
namespace PeripheralMessages {

struct PayloadHeader
{
	MessageId mMessageId;
	uint16_t mReserved; /*reserved? maybe version*/
};

template<MessageId _messageId, class _messageStruct>
class Message
{
public:

	Message() :
	mMessageAllocated(true)
	{
		//boo this doesnt work on MSP430 for some reason, malloc as well...
		mMessageBuffer = new uint8_t[sizeof(PayloadHeader)+sizeof(_messageStruct)];
		Initialize(mMessageBuffer,sizeof(PayloadHeader)+sizeof(_messageStruct),true);
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
		mMessageBuffer = buffer;
		Initialize(buffer,bufferLength,createMessage);
	}


	virtual ~Message()
	{
		if(mMessageAllocated) {
			mMessageAllocated = false;
			delete[] mMessageBuffer;
		}
	}
	
	static void set_callback(callback cb)
	{
		mCallback = cb;
	}

	static void default_callback(void* msg, uint16_t calling_id) {
		//Do nothing
	}
	
	static void trigger_callback(void* msg, uint16_t calling_id)  //add default callback that warns in compiler? or Assert to force it.
	{
		mCallback(msg,calling_id);
	}
	
	
	const size_t get_message_buffer_size()
	{
		return sizeof(PayloadHeader)+sizeof(_messageStruct);
	}

	uint8_t* const get_message_buffer() const
	{
		return mMessageBuffer;
	}
	
	
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
			mMessageBuffer = nullptr;
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
	uint8_t* mMessageBuffer;
    PayloadHeader* mHeader;
	_messageStruct* mMessage;
	static callback mCallback;

};
template<MessageId m, class t>
callback Message<m,t>::mCallback = Message<m,t>::default_callback;
template<MessageId m, class t>


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


	
	

}
#endif //MESSAGE_HPP_

