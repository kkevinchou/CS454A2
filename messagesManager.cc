#include "messagesManager.h"
#include "message.h"
#include <sys/socket.h>
#include <iostream>
#include <stdlib.h> 


MessagesManager * instance = NULL;
static void * sendMessages(void * p)
{

    int n;

	while(true)
	{
		//cout << "? " <<endl;
		if(instance == NULL) cout << "INSTANCE IS NULL"<<endl;
		if(instance != NULL && instance->_m.size() > 0)
		{
			//cout << "has stuff"<<endl;
			Message * msg = NULL;
			pthread_mutex_lock(instance->_listLock);
			msg = instance->_m.front();
			instance->_m.pop_front();
			pthread_mutex_unlock(instance->_listLock);

			if(msg)
			{
				//cout << "sending..."<<endl;
				string s = msg->getString();

				  // cout << "Please enter the message: ";
		       // memset(buffer,0,256);
		       // fgets(buffer,255,stdin);

		       // if(buffer == NULL) break;

		        n = send(instance->_sfd,s.c_str(),s.size(), 0);
		      //  cout << "Sent"<<endl;
		        if (n < 0)
		        {
		            cerr << "ERROR writing to socket for message: "<<s << endl;
				}


			}
		}

         sleep(2);



	}
	return NULL;
}


MessagesManager::MessagesManager(int socketFileDescriptor) : _sfd(socketFileDescriptor)
{
	// spawn thread that processes _m
	//cout << "1"<<endl;
	_listLock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	 pthread_mutex_init(_listLock, NULL);
	// cout << "2"<<endl;
	 //pthread_t receivingThread;
	// cout << "3"<<endl;
	 pthread_create(&_thread, NULL, &sendMessages,NULL);
	// cout << "4"<<endl;
}

void MessagesManager::addMessage(string m)
{
	Message * msg = new Message(m);

	pthread_mutex_lock(_listLock);
	_m.push_back(msg);
	pthread_mutex_unlock(_listLock);
}

MessagesManager::~MessagesManager()
{
	pthread_mutex_destroy(_listLock);
}
