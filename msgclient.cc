#include "msgclient.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "logger.h"

MsgClient::MsgClient(string host, int port) : socket(host, port) {}

MsgClient::~MsgClient() {
}

int MsgClient::run()
{
    // run the client state machine
    while (true) {
        // handle dead sockets etc. through all stages
        int err = 0;
        // have a consistent request through all states and processing
        Request req;
        req.type = BAD_REQ;

        // build request
        err = runCommandLine(req);
        if (err < 0) {
            LOG("runCommandLine gave an error (#%d), stopping running client.\n", err);
            return err;
        }
        if (req.type == BAD_REQ) {
            continue;
        }
        err = send_request(req);
        if (err < 0) {
            LOG("send_request gave an error (#%d), stopping running client.\n", err);
            return err;
        }
        err = get_response(req);
        if (err < 0) {
            LOG("get_response gave an error (#%d), stopping running client.\n", err);
            return err;
        }
    }
}

int
MsgClient::runCommandLine(Request &req) {
    string lineStr;

    // loop to get first line of request
    while (getline(cin,lineStr)) {
        stringstream line(lineStr);

        string verb;
        line >> verb;
        if (verb == "list") {
            req.type = LIST;
            break;
        }
        LOG("Command line verb was %s\n", verb.c_str());
    }
}

int
MsgClient::send_request(Request &req)
{
    string request = "list jon";
    //prepare_request(req, request);
    int err = socket.sendString(request);
    if (err <= 0) {
        return -1;
    }
}

int
MsgClient::get_response(Request &req) {
    string response;
    socket.readToSentinel("\n", response);
}
