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
		//if(instance == NULL) cout << "INSTANCE IS NULL"<<endl;
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
				string s = msg->getString();

				unsigned int size = s.size()+1;
				const char * cstr = s.c_str();

				unsigned char sizeBytes[4];
				unsigned char * sizeBytesP = sizeBytes;

				sizeBytes[0] = (size >> 24) & 0xFF;
				sizeBytes[1] = (size >> 16) & 0xFF;
				sizeBytes[2] = (size >> 8) & 0xFF;
				sizeBytes[3] = size & 0xFF;

				int sizeSize = 4;

				while(true)
				{
					//send the first 4 bytes
					// keep sending until send returns 0
					n = send(instance->_sfd,sizeBytesP, sizeSize, 0);
					if(n==0)
					{
						break;
					}
			        else if (n < 0)
			        {
			            cerr << "ERROR writing to socket for message: "<<s << endl;
			            exit(-1);
					}

					sizeSize -= n;
					sizeBytesP += n;

					sleep(2);
				}

				while(true)
				{
					// send the message
					// keep sending until send returns 0
					n = send(instance->_sfd,cstr, size, 0);
					if(n==0)
					{
						break;
					}
			        else if (n < 0)
			        {
			            cerr << "ERROR writing to socket for message: "<<s << endl;
			            exit(-1);
					}


					size -= n;
					cstr += n;

					sleep(2);
				}
		        
				delete msg;

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
	free(_listLock);
}
