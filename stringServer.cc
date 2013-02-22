#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>

using namespace std;

void error(string message)
{
    cerr << message << endl;
    exit(-1);
}

void printServerSettings(int localSocketFd) {
    char localHostName[256];
    gethostname(localHostName, 256);
    cout << "SERVER_ADDRESS " << localHostName << endl;

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    getsockname(localSocketFd, (struct sockaddr *)&sin, &len);
    cout << "SERVER_PORT " << ntohs(sin.sin_port) << endl;
}

int waitForConnection(int localSocketFd) {
    struct sockaddr_in serverAddress;
    memset((struct sockaddr_in *)&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = 0;

    if (bind(localSocketFd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
          error("ERROR: Failed to bind local socket");

    listen(localSocketFd, 5);
    printServerSettings(localSocketFd);

    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int newSocketFd = accept(localSocketFd, (struct sockaddr *) &clientAddress, &clientAddressSize);

    if (newSocketFd < 0)
        error("ERROR: Failed to accept client connection");

    return newSocketFd;
}

int acceptConnection(int localSocketFd) {
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int newSocketFd = accept(localSocketFd, (struct sockaddr *) &clientAddress, &clientAddressSize);

    if (newSocketFd < 0)
        error("ERROR: Failed to accept client connection");

    return newSocketFd;
}

void listenOnSocket(int localSocketFd) {
    struct sockaddr_in serverAddress;
    memset((struct sockaddr_in *)&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = 0;

    if (bind(localSocketFd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
          error("ERROR: Failed to bind local socket");

    listen(localSocketFd, 5);
}

int main(int argc, char *argv[])
{
    int localSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    cerr << "LOCAL SOCKET: " << localSocketFd << endl;
    if (localSocketFd < 0) {
        error("ERROR: Failed to open socket");
    }

    listenOnSocket(localSocketFd);
    printServerSettings(localSocketFd);

    int max_fd = localSocketFd;
    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    FD_SET(localSocketFd, &master_set);

    timeval timeout;
    timeout.tv_sec = 3 * 60;
    timeout.tv_usec = 0;

    while (true) {
        memcpy(&working_set, &master_set, sizeof(master_set));
        int selectResult = select(max_fd + 1, &working_set, NULL, NULL, &timeout);

        if (selectResult < 0) {
            error("ERROR: Select failed");
        } else if (selectResult == 0) {
            error("ERROR: Select timed out");
        }

        for (int i = 0; i < max_fd + 1; i++) {
            if (FD_ISSET(i, &working_set)) {
                if (i != localSocketFd) {
                    int clientSocketFd = i;
                    char buffer[256];
                    memset(buffer, 0, 256);
                    int ioStatus = recv(clientSocketFd, buffer, 255, 0);

                    if (ioStatus == 0) {
                        FD_CLR(clientSocketFd, &master_set);
                        close(clientSocketFd);
                        continue;
                    }

                    if (ioStatus < 0) {
                        error("ERROR: Failed to read from socket");
                    }

                    ioStatus = write(clientSocketFd, "I got your message", 18);

                    if (ioStatus < 0) {
                        error("ERROR: Failed to write to socket");
                    }

                    cout << "MESSAGE: " << buffer << endl;
                } else {
                    int newSocketFd = acceptConnection(localSocketFd);
                    max_fd = newSocketFd;
                    FD_SET(newSocketFd, &master_set);
                    cerr << "CONNECTION RECEIVED " << newSocketFd << endl;
                }
            }
        }
    }

    close(localSocketFd);

    return 0;
}