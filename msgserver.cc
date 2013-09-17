#include "msgserver.h"

// parsing helps
#include <sstream>
#include <cstdlib>

// debugging-toggled logging facility
#include "logger.h"

// definitions for working the protocol
//#include "proto.h"

#define DEFAULT_MAX_CONNS 128

MsgServer::MsgServer(int port) : serverSocket(port, DEFAULT_MAX_CONNS)
{
    LOG("MsgServer(int %d)\n", port);
}

MsgServer::~MsgServer() {}

int MsgServer::listen()
{
    int client;
    // accept clients
    while ((client = serverSocket.acceptClient()) > 0) {
        LOG("Hey, a client!\n");
        Socket clientSocket(client);
        serveClient(clientSocket);
        // destructor of Socket automagically closes socket
    }
}

int MsgServer::serveClient(Socket &sock)
{
    // wawoo! we now keep reading from this socket until it dies.
    while (1) {
        string rawReq;
        int err;

        // first, wait for a \n-delimited command from the new client
        err = sock.readToSentinel("\n", rawReq);
        // stop reading on the socket if it has closed or errored
        if (err <= 0) {
            return err;
        }

        // now that we have a shiny command of some kind, parse it.

        stringstream stream(rawReq);
        string verb, response = "OK\n";
        stream >> verb;
        if (verb == "put") {
            string user, subject, lenStr, message;
            int len;

            stream >> user;
            stream >> subject;
            stream >> lenStr;
            // if the client didn't give us good data, complain.
            if (user == "" || subject == "" || lenStr == "") {
                LOG("Missing PUT string\n");
                response = "error - Missing field in PUT request\n";
                err = sock.sendString(response);
                if (err <= 0) {
                    return err;
                }
                continue;
            }

            len = atoi(lenStr.c_str());
            err = sock.readNBytes(len, message);
            if (err <= 0) {
                return err;
            }

            // store parsed message
            //TODO

            LOG("Got message.\n\tName: %s\n\tSubject: %s\n\tMessage Length: %d\n\tMessage: '%s'\n", user.c_str(), subject.c_str(), len, message.c_str());
        }// else if ()
    }
}

