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
#include <sstream>

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

void toTitleCase(string &inStr) {
    bool capNext = true;

    for (unsigned int i = 0; i < inStr.length(); i++) {
        if (capNext && inStr[i] >= 97 && inStr[i] <= 122) {
            inStr[i] -= 32;
        }

        if (inStr[i] == 32) {
            capNext = true;
        } else {
            capNext = false;
        }
    }
}

string getStringFromBuffer(char buffer[], int n) {
    char charStr[n];
    for (int i = 0; i < n - 1; i++) {
        charStr[i] = buffer[i];
    }
    charStr[n - 1] = '\0';

    string result = charStr;
    return result;
}

void handleRequest(int clientSocketFd, fd_set *master_set) {
    char buffer[256];
    memset(buffer, 0, 256);
    int ioStatus = recv(clientSocketFd, buffer, 255, 0);

    if (ioStatus == 0) {
        FD_CLR(clientSocketFd, master_set);
        close(clientSocketFd);
        return;
    }

    if (ioStatus < 0) {
        error("ERROR: Failed to read from socket");
    }

    string recvStr = getStringFromBuffer(buffer, ioStatus);
    string processedStr = recvStr;

    cout << recvStr << endl;

    toTitleCase(processedStr);

    std::ostringstream ss;
    ss << processedStr;

    ioStatus = send(clientSocketFd, ss.str().c_str(), processedStr.length() + 1, 0);

    if (ioStatus < 0) {
        error("ERROR: Failed to write to socket");
    }
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

    while (true) {
        memcpy(&working_set, &master_set, sizeof(master_set));
        int selectResult = select(max_fd + 1, &working_set, NULL, NULL, NULL);

        if (selectResult < 0) {
            error("ERROR: Select failed");
        } else if (selectResult == 0) {
            error("ERROR: Select timed out");
        }

        for (int i = 0; i < max_fd + 1; i++) {
            if (FD_ISSET(i, &working_set)) {
                if (i != localSocketFd) {
                    int clientSocketFd = i;
                    handleRequest(clientSocketFd, &master_set);
                } else {
                    int newSocketFd = acceptConnection(localSocketFd);
                    max_fd = newSocketFd;
                    FD_SET(newSocketFd, &master_set);
                }
            }
        }
    }

    close(localSocketFd);

    return 0;
}