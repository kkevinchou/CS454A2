#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

using namespace std;
class Message
{
	private:
		string _msg;
		bool _validString;
		unsigned int _sizeLeft;

	public:
		Message(string message);
		Message(int size);
		string getString();
		bool addToString(string partialString);

		bool isComplete;
};

#endif