#ifndef MSGSERVER_H
#define MSGSERVER_H

#include "socket.h"

// DB storage
#include <map>
#include <vector>
#include <utility>

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
    map <string, vector <pair <string, string> > > db;
};

#endif // MSGSERVER_H
