#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <cstring>

using namespace std;

void error(string message)
{
    cerr << message << endl;
    exit(-1);
}

int listenForConnection(int localSocketFd, int port) {
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(localSocketFd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
          error("ERROR on binding");

    listen(localSocketFd, 5);

    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int newSocketFd = accept(localSocketFd, (struct sockaddr *) &clientAddress, &clientAddressSize);

    if (newSocketFd < 0)
        error("ERROR on accept");

    return newSocketFd;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        error("PORT NUMBER MISSING");
    }

    int localSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (localSocketFd < 0) {
        error("ERROR opening socket");
    }

    int port = atoi(argv[1]);
    int newSocketFd = listenForConnection(localSocketFd, port);

    char buffer[256];
    memset(buffer, 0, 256);
    int ioStatus = read(newSocketFd, buffer, 255);

    if (ioStatus < 0) error("ERROR reading from socket");

    cout << "MESSAGE: " << buffer << endl;

    ioStatus = write(newSocketFd, "I got your message", 18);

    if (ioStatus < 0) {
        error("ERROR writing to socket");
    }

    close(newSocketFd);
    close(localSocketFd);

    return 0;
}