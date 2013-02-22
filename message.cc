#include "message.h"

Message::Message(string message)
{
	_msg = message;
}

string Message::getString()
{
	return _msg;
}