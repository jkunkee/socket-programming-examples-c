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

    LOG("Now handling new command-line input!\n");

    // loop to get first line of request
    while (true) {
        // display prompt
        cout << "% ";

        // read in one line of input
        getline(cin, lineStr);

        // parse it
        stringstream line(lineStr);

        string verb;
        line >> verb;

        if (verb == "send") {
            line >> req.name;
            line >> req.subject;

            if (req.name == "") {
                LOG("send: no name found.");
                continue;
            }
            if (req.subject == "") {
                LOG("send: no subject found.");
                continue;
            }

            req.message = "";
            while (getline(cin, lineStr)) {
                if (lineStr == "") {
                    LOG("End of message found!\n");
                    break;
                }
                req.message += lineStr;
                req.message += "\n";
            }

            req.type = PUT;
            break;
        } else if (verb == "list") {
            line >> req.name;
            if (req.name == "") {
                LOG("usage: list <username>");
                continue;
            }
            req.type = LIST;
            break;
        } else if (verb == "read") {
            line >> req.name;
            line >> req.index;
            if (line == "") {
                LOG("read command: no user name found");
                continue;
            }
            if (req.index < 1) {
                LOG("%d is a bad message index. (they're 1-indexed.)\n", req.index);
                continue;
            }
            req.type = GET;
            break;
        } else if (verb == "quit") {
            LOG("Received 'quit' command. Halting event loop.\n");
            return -1;
        } else {
            LOG("'%s' is an invalid verb. Valid verbs are: send, list, read, quit\n", verb.c_str());
        }
    }
    return 0;
}

void MsgClient::prepare_request(Request &req, string &request)
{
    // start from scratch
    request = "";
    stringstream stream(request);

    switch (req.type) {
    case PUT:
        stream << "put " << req.name << " " << req.subject << " " << req.message.length() << "\n";
        stream << req.message;
        break;
    case LIST:
        stream << "list ";
        stream << req.name;
        stream << "\n";
        break;
    case GET:
        stream << "get " << req.name << " " << req.index << "\n";
        break;
    default:
        break;
    }
    request = stream.str();
}

int
MsgClient::send_request(Request &req)
{
    string request;
    prepare_request(req, request);

    int err = socket.sendString(request);
    if (err <= 0) {
        return -1;
    } else {
        return 0;
    }
}

int
MsgClient::get_response(Request &req) {
    string response;
    int err;

    // the first thing to come back is always terminated with a newline
    err = socket.readToSentinel("\n", response);
    if (err <= 0) {
        LOG("Error while reading server's response: %d\n", err);
        return -1;
    }

    // parse out the verb (same idea as in reading from the command line)
    stringstream stream(response);
    string verb;
    stream >> verb;

    if (verb == "OK") {
        cout << "OK" << endl;
    } else if (verb == "error") {
        cout << "error " << stream.str() << endl;
    } else if (verb == "list") {
        int msgCount;
        stream >> msgCount;
        for (/*already initialized*/; msgCount > 0; msgCount--) {
            err = socket.readToSentinel("\n", response);
            if (err <= 0) {
                LOG("Error while reading server's LIST response: %d\n", err);
                return -1;
            }
            cout << response << endl;
        }
    } else if (verb == "message") {
        string subject;
        int msgLen;
        string message;

        stream >> subject;
        stream >> msgLen;

        err = socket.readNBytes(msgLen, message);
        if (err <= 0) {
            LOG("Error while reading server's GET response: %d\n", err);
            return -1;
        }

        cout << subject << endl;
        cout << message << endl;
    }

    return 0;
}
