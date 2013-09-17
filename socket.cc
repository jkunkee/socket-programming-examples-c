#include "socket.h"

// for buffering
#include <sstream>

// for the socket (copied from Zappala's code)
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

// for debugging
#include "logger.h"

// make a new server
Socket::Socket(int srvPort, int maxConns)
{
    bufLen = DEFAULT_BUFFER_LENGTH;
    buf = new char[bufLen+1];
    LOG("Successfully allocated new buffer!\n");

    LOG("Creating new server/listening socket\n");
    sockFd = 0;

    struct sockaddr_in server_addr;

    // setup socket address structure
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(srvPort);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // create socket
    sockFd = socket(PF_INET,SOCK_STREAM,0);
    if (!sockFd) {
        perror("socket");
        exit(-1);
    }

    // set socket to immediately reuse port when the application closes
    int reuse = 1;
    if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt");
        exit(-1);
    }

    // call bind to associate the socket with our local address and
    // port
    if (bind(sockFd,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        perror("bind");
        exit(-1);
    }

      // convert the socket to listen for incoming connections
    if (listen(sockFd,SOMAXCONN) < 0) {
        perror("listen");
        exit(-1);
    }
}

Socket::Socket(int fd)
{
    bufLen = DEFAULT_BUFFER_LENGTH;
    buf = new char[bufLen+1];
    LOG("Successfully allocated new buffer!\n");
    LOG("wrapping extant socket #%d\n", fd);
    sockFd = fd;
}

Socket::~Socket()
{
    if (sockFd > 0) {
        close(sockFd);
    }
    delete buf;
}

int Socket::readToSentinel(const char *sentinel, string &targ)
{
    int idx, nread;
    while ((idx = cache.find(sentinel, 0)) == string::npos) {

        LOG("Trying to read off of socket\n");
        nread = recv(sockFd, buf, bufLen, 0);

        if (nread < 0) {
            if (errno == EINTR) {
                // the socket call was interrupted -- try again
                LOG("Socket call interrupted\n");
                continue;
            } else {
                perror("while scanning for sentinel");
                LOG("saw error on recv: %d\n", errno);
                // an error occurred, stop trying to recv()
                return nread;
            }
        } else if (nread == 0) {
            // socket closed, stop trying to recv()
            LOG("Child socket closed during sentinel search loop\n");
            return nread;
        } else {
            LOG("got data");
            // the socket gave a strictly positive return; use it
            cache.append(buf, 0, nread);
        }
    }
    // idx contains the index of the sentinel
    //copy out the useful data
    targ = cache.substr(0, idx);
    // remove it from the cache, preserving any extra data (except the delimiter/sentinel)
    cache.erase(0, idx+1);

    LOG("Using delim '%s' I found '%s'\n", sentinel, targ.c_str());
    return nread;
}

int Socket::readNBytes(int n, string &targ)
{
    int idx, nread;
    while (cache.length() < n) {

        LOG("Trying to read off of socket\n");
        nread = recv(sockFd, buf, bufLen, 0);

        if (nread < 0) {
            if (errno == EINTR) {
                // the socket call was interrupted -- try again
                LOG("Socket call interrupted\n");
                continue;
            } else {
                perror("while scanning for sentinel");
                LOG("saw error on recv: %d\n", errno);
                // an error occurred, stop trying to recv()
                return nread;
            }
        } else if (nread == 0) {
            // socket closed, stop trying to recv()
            LOG("Child socket closed during sentinel search loop\n");
            return nread;
        } else {
            // the socket gave a strictly positive return; use it
            cache.append(buf, 0, nread);
        }
    }
    // idx contains the index of the sentinel
    //copy out the useful data
    targ = cache.substr(0, n);
    // remove it from the cache, preserving any extra data
    cache.erase(0, n);

    LOG("Using count '%d' I found '%s'\n", n, targ.c_str());
    return nread;
}

int Socket::sendString(string &response)
{
    // prepare to send response
    const char* ptr = response.c_str();
    int nleft = response.length();
    int nwritten;
    LOG("Sending response: '%s'\n", response.c_str());
    // loop to be sure it is all sent
    while (nleft) {
        if ((nwritten = send(sockFd, ptr, nleft, 0)) < 0) {
            if (errno == EINTR) {
                // the socket call was interrupted -- try again
                continue;
            } else {
                // an error occurred, so break out
                perror("write");
                return false;
            }
        } else if (nwritten == 0) {
            // the socket is closed
            return false;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    LOG("response sent.\n");
    return true;
}

int Socket::acceptClient()
{
    // setup client
    int client;
    struct sockaddr_in client_addr;
    socklen_t clientlen = sizeof(client_addr);
    client = accept(sockFd, (struct sockaddr *)&client_addr, &clientlen);
    return client;
}

