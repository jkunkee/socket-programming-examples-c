#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include "proto.h"
#include "infinibuf.h"

using namespace std;

class Server {
public:
    Server(int);
    Server(int, bool);
    ~Server();

private:
    void create();
    void serve();
    void handle(int);
    Request collect_request(int);
    bool send_response(int, string);

    int port_;
    int server_;
    InfiniBuffer buf_;
    bool debug_;
};
