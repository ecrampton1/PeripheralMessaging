#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_
#include "messaging.hpp"


using callback = void (*)(void*);
namespace PeripheralMessages {

struct PayloadHeader
{
	MessageId mMessageId;
	uint8_t mReserved; /*reserved? maybe version*/
};

template<MessageId _messageId, class _messageStruct>
class Message
{
public:
	
	Message() :
	mMessageAllocated(true)
	{
		mMessageBuffer = new uint8_t[sizeof(PayloadHeader)+sizeof(_messageStruct)];
		Message(mMessageBuffer,sizeof(PayloadHeader)+sizeof(_messageStruct));
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
		Initialize(buffer,bufferLength,createMessage);
	}


	virtual ~Message()
	{
		if(mMessageAllocated) {
			mMessageAllocated = false;
			delete[] mMessageBuffer;
		}
	}
	
	static void set_callback(callback cb, void* ca)
	{
		mCallback = cb;
		mCallbackArgs = ca;
	}

	static void default_callback(void*) {
		//Do nothing
	}
	
	void trigger_callback()  //add default callback that warns in compiler? or Assert to force it.
	{
		mCallback(mCallbackArgs);
	}
	
	
	const size_t get_message_buffer_size()
	{
		return sizeof(PayloadHeader)+sizeof(_messageStruct);
	}

	const uint8_t* get_message_buffer()
	{
		return mMessageBuffer;
	}
	
	
	const PayloadHeader* get_message_header()
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
using VersionQueryMsg = Message<MessageId::VersionQuery,VersionMessage>;





	
	

};
#endif //MESSAGE_HPP_

