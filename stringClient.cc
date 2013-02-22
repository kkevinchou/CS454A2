#include <iostream>
#include <string>

#include "message.h"
#include <list>




#include <cstdlib>
#include <stdio.h>
#include <stdlib.h> //getenv
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <cstring>

using namespace std;
void error(string msg)
{
    cerr << msg << endl;
    exit(-1);
}


int setupSocketAndReturnDescriptor(char * serverAddressString, char * serverPortString)
{
    int socketFileDescriptor;
    int serverPort;

    struct sockaddr_in serverAddressStruct;
    struct hostent *server;

 

    serverPort = atoi(serverPortString);
    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFileDescriptor < 0)
        error("ERROR while opening socket");

    server = gethostbyname(serverAddressString);

    if (server == NULL) {
        error("ERROR: No such host");

    }


    memset((char *) &serverAddressStruct, 0,sizeof(serverAddressStruct));

    serverAddressStruct.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serverAddressStruct.sin_addr.s_addr,
         server->h_length);
    serverAddressStruct.sin_port = htons(serverPort);

    cout << "HOST: "<<serverAddressString << " PORT: " << serverPort<<endl;

    if (connect(socketFileDescriptor,(struct sockaddr *) &serverAddressStruct,sizeof(serverAddressStruct)) < 0)
        error("ERROR while connecting");

    return socketFileDescriptor;
}
int main(int argc, char *argv[])
{
    char * serverAddressString = getenv ("SERVER_ADDRESS");
    char * serverPortString = getenv("SERVER_PORT");

    if(serverAddressString == NULL) error("ERROR: SERVER_ADDRESS environment variable not set.");
    if(serverPortString == NULL) error("ERROR: SERVER_PORT environment variable not set.");

    int socketFileDescriptor = setupSocketAndReturnDescriptor(serverAddressString, serverPortString);

    char buffer[256];
    int n;

	//list<Message* > msges;

	while(true)
	{
		//Message * m = new Message(msg);
		//msges.push_front(m);

        cout << "Please enter the message: ";
        memset(buffer,0,256);
        fgets(buffer,255,stdin);

        if(buffer == NULL) break;

        n = write(socketFileDescriptor,buffer,strlen(buffer));
        if (n < 0)
             error("ERROR writing to socket");
        memset(buffer,0,256);
        n = read(socketFileDescriptor,buffer,255);
        if (n < 0)
             error("ERROR reading from socket");
        cout << buffer << endl;
	}

	// eof, but wait for server to respond

    close(socketFileDescriptor);
}