#include "message.h"

Message::Message(string message)
{
	_msg = message;
}

string Message::getString()
{
	return _msg;
}

Message::Message()
{
	_msg = "";
}

void Message::addToString(string partialString)
{
	_msg = _msg + partialString;
}
