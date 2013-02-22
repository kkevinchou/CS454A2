#include "messagesManager.h"
#include "message.h"
MessagesManager::MessagesManager(int socketFileDescriptor) : _listLock(PTHREAD_MUTEX_INITIALIZER), _sfd(socketFileDescriptor)
{
	// spawn thread that processes _m

}

void MessagesManager::addMessage(string m)
{
	Message * msg = new Message(m);
	_m.push_back(msg);
}