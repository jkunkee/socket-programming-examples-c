#include <stdlib.h>
#include <unistd.h>

#include <iostream>

#include "msgclient.h"
#include "logger.h"

using namespace std;

int
main(int argc, char **argv)
{
    int option;

    // setup default arguments
    int port = 3000;
    string host = "localhost";

    Logger::getInstance()->doNotLog();

    // process command line options using getopt()
    // see "man 3 getopt"
    while ((option = getopt(argc,argv,"dh:p:")) != -1) {
        switch (option) {
            case 'p':
                port = atoi(optarg);
                break;
            case 'h':
                host = optarg;
                break;
            case 'd':
                Logger::getInstance()->doLog();
                break;
            default:
                cout << "client [-h host] [-p port] [-d]" << endl;
                exit(EXIT_FAILURE);
        }
    }

    LOG("\nWelcome to msg client vJon\n\n");

    MsgClient client(host, port);
    client.run();
}

