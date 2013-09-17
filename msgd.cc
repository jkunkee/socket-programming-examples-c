#include <stdlib.h>
#include <unistd.h>

#include <iostream>

#include "msgserver.h"
#include "logger.h"

using namespace std;

int
main(int argc, char **argv)
{
    int option, port;

    // setup default arguments
    port = 3000;
    Logger::getInstance()->doNotLog();

    // process command line options using getopt()
    // see "man 3 getopt"
    while ((option = getopt(argc,argv,"p:d")) != -1) {
        switch (option) {
            case 'p':
                port = atoi(optarg);
                break;
            case 'd':
                Logger::getInstance()->doLog();
                break;
            default:
                cout << "server [-p port]" << endl;
                exit(EXIT_FAILURE);
        }
    }
    LOG("\nWelcome to Msgd vJon\n\n");

    MsgServer server(port);
    return server.listen();
}
