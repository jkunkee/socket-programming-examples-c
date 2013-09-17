#ifndef MSGSERVER_H
#define MSGSERVER_H

#include "socket.h"

class MsgServer
{
public:
    MsgServer(int port);
    ~MsgServer();

    // primary event loop
    int listen();
private:
    MsgServer();

    int serveClient(Socket& sock);

    // fields
    Socket serverSocket;
    //map db;
};

#endif // MSGSERVER_H
