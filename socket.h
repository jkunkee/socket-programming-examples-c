#ifndef SOCKET_H
#define SOCKET_H

#include <string>

#define DEFAULT_BUFFER_LENGTH 1024

using namespace std;

enum SOCKET_TYPE { SERVER, CLIENT };

class Socket
{
public:
    // wrap a normal socket
    Socket(int fd);
    // create a listening socket
    Socket(int port, int maxConns);
    // make a connection
    Socket(string host_, int port_);
    // I am become XYXY, Deallocator of Memory
    ~Socket();

    // stream readers
    int readToSentinel(const char *sentinel, string& targ);
    int readNBytes(int n, string& targ);
    // stream writers
    int sendString(string& msg);

    // only for server sockets!
    int acceptClient();
private:
    Socket();

    // fields
    char* buf;
    int bufLen;
    int sockFd;
    string cache;
};

#endif // SOCKET_H
