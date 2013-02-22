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

int listenForConnection(int localSocketFd) {
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = 0;

    if (bind(localSocketFd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
          error("ERROR: Failed to bind local socket");

    listen(localSocketFd, 5);

    char localHostName[256];
    gethostname(localHostName, 256);
    cout << "SERVER_ADDRESS " << localHostName << endl;

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    getsockname(localSocketFd, (struct sockaddr *)&sin, &len);
    cout << "SERVER_PORT " << sin.sin_port << endl;

    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int newSocketFd = accept(localSocketFd, (struct sockaddr *) &clientAddress, &clientAddressSize);

    if (newSocketFd < 0)
        error("ERROR: Failed to accept client connection");

    return newSocketFd;
}

int main(int argc, char *argv[])
{
    int localSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (localSocketFd < 0) {
        error("ERROR: Failed to open socket");
    }

    int newSocketFd = listenForConnection(localSocketFd);

    char buffer[256];
    memset(buffer, 0, 256);
    int ioStatus = read(newSocketFd, buffer, 255);

    if (ioStatus < 0) {
        error("ERROR: Failed to read from socket");
    }

    cout << "MESSAGE: " << buffer << endl;

    ioStatus = write(newSocketFd, "I got your message", 18);

    if (ioStatus < 0) {
        error("ERROR: Failed to write to socket");
    }

    close(newSocketFd);
    close(localSocketFd);

    return 0;
}