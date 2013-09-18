
#ifndef SNTNL_CLIENT_H
#define SNTNL_CLIENT_H

#include <string>

#include "proto.h"
#include "socket.h"

using namespace std;

class MsgClient {
public:
    MsgClient(string, int);
    ~MsgClient();

    int run();
private:
    MsgClient();

    int runCommandLine(Request &req);
    void prepare_request(Request &req, string &request);
    int send_request(Request &req);
    int get_response(Request &req);

    Socket socket;
};

#endif // CLIENT_H
