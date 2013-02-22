#include <iostream>
#include <string>
#include <sys/socket.h>

#include "message.h"
#include <list>

using namespace std;


int main()
{
	string msg;

	list<Message> msges;

	while(cin>>msg)
	{
		Message m = new Message(msg);
		msges.push_front(m);
	}


	// eof, but wait for server to respond
}