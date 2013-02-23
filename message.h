#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

using namespace std;
class Message
{
	private:
		string _msg;

	public:
		Message(string message);
		Message();
		string getString();
		void addToString(string partialString);

};

#endif