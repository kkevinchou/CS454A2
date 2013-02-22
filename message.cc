#include "message.h"

Message::Message(string message)
{
	_msg = message;
	_sizeLeft = 0;
	isComplete = true;
}

string Message::getString()
{
	return _msg;
}

Message::Message(int size)
{
	_sizeLeft = size;
	isComplete = false;
}

bool Message::addToString(string partialString)
{
	// this message content is already complete
	if(_sizeLeft <= 0)
	{
		isComplete = true;
		return false;
	} 

	// the partial string contents are too big to fit, so they are probably not right
	if(partialString.size() > _sizeLeft) return false;

	_msg = _msg + partialString;
	_sizeLeft -= partialString.size();
	if(_sizeLeft <= 0) isComplete = true;
	return true;



}
