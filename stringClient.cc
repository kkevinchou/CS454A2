#include <iostream>
#include <string>

#include "messagesManager.h"
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

#include <pthread.h>

using namespace std;

extern MessagesManager * instance;

volatile int messageCounter = 0;
volatile bool inputDone = false;

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

   // cout << "HOST: "<<serverAddressString << " PORT: " << serverPort<<endl;

    if (connect(socketFileDescriptor,(struct sockaddr *) &serverAddressStruct,sizeof(serverAddressStruct)) < 0)
        error("ERROR while connecting");

    return socketFileDescriptor;
}


void *sendInputToSocket(void *fdp)
{

    string line;

    while(cin >> line)
    {
        instance->addMessage(line);
        messageCounter++;
    }

    return NULL;
}

void *receiveFromSocketAndSendToOutput(void *fdp)
{
    int socketFileDescriptor = *(int *)(fdp);
    
    int n;

    while(true)
    {
        unsigned char sizeBuffer[4];
        unsigned int sizeSize = 4;
        unsigned char * sizeBufferP = sizeBuffer;
        while(true)
        {
            memset(sizeBuffer,0,4);
            n = recv(socketFileDescriptor,sizeBufferP,sizeSize, 0);
            if(n==0)
            {
                return NULL; //connection closed!
            }
            else if (n < 0)
                 error("ERROR reading from socket");

             sizeBufferP += n;
             sizeSize -= n;

             if(sizeSize == 0) break;
            //receive 4 bytes first
        }

        unsigned int messageSize = (sizeBuffer[0] << 24) + (sizeBuffer[1] << 16) + (sizeBuffer[2] << 8) + sizeBuffer[3];
        Message * m = new Message(messageSize-1); // size ignores null char
        char buffer[messageSize];
        while(true)
        {
          // receive the message
            memset(buffer,0,messageSize);
            n = recv(socketFileDescriptor,buffer,messageSize, 0);
           // cout << "got something"<<endl;
            if(n == 0)
            {
                return NULL; //connection closed!
            }
            else if (n < 0)
                 error("ERROR reading from socket");

             string line(buffer);
             m->addToString(line);

             messageSize -= n;
             if(messageSize <= 0) break;

        }

        cout << "Server: " << m->getString()<<endl;
        delete m;
    }

    return NULL;

}

int main(int argc, char *argv[])
{
    char * serverAddressString = getenv ("SERVER_ADDRESS");
    char * serverPortString = getenv("SERVER_PORT");

    if(serverAddressString == NULL) error("ERROR: SERVER_ADDRESS environment variable not set.");
    if(serverPortString == NULL) error("ERROR: SERVER_PORT environment variable not set.");

    int socketFileDescriptor = setupSocketAndReturnDescriptor(serverAddressString, serverPortString);

   /* char buffer[256];
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

        n = send(socketFileDescriptor,buffer,strlen(buffer), 0);
        cout << "Sent"<<endl;
        if (n < 0)
             error("ERROR writing to socket");
        memset(buffer,0,256);
        n = recv(socketFileDescriptor,buffer,255, 0);
        if (n < 0)
             error("ERROR reading from socket");
        cout << buffer << endl;
    }*/

    // eof, but wait for server to respond

    //close(socketFileDescriptor);

        instance = new MessagesManager(socketFileDescriptor);

        pthread_t sendingThread ;
       // pthread_t receivingThread;

   // pthread_create(&receivingThread, NULL, &receiveFromSocketAndSendToOutput, &socketFileDescriptor);
        //cout << "q"<<endl;
    pthread_create(&sendingThread, NULL, &sendInputToSocket, &socketFileDescriptor);

     //pthread_join(sendingThread, NULL); //
       // sendInputToSocket(&socketFileDescriptor);
        receiveFromSocketAndSendToOutput(&socketFileDescriptor);
}