#ifndef MESSAGESMANAGER_H
#define MESSAGESMANAGER_H

#include <string>
#include <list>
#include "message.h"

using namespace std;


class MessagesManager
{
	private:
		 
		 
		 

		//static void * sendMessages(void * p);
		 ~MessagesManager();

	public:
		 MessagesManager(int socketFileDescriptor);
		 void addMessage(string m);
		 list<Message *> _m;
		 int _sfd;
		 pthread_mutex_t * _listLock;
		 pthread_t _thread;

//
};



#endif