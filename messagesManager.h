#ifndef MESSAGESMANAGER_H
#define MESSAGESMANAGER_H

#include <string>
#include <list>
#include "message.h"

using namespace std;
class MessagesManager
{
	private:
		pthread_mutex_t _listLock;
		int _sfd;
		list<Message *> _m;

	public:
		MessagesManager(int socketFileDescriptor);
		void addMessage(string m);
};

#endif