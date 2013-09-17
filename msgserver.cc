#include "msgserver.h"

// parsing helps
#include <sstream>
#include <cstdlib>

// debugging-toggled logging facility
#include "logger.h"

// definitions for working the protocol
//#include "proto.h"

#define DEFAULT_MAX_CONNS 128

MsgServer::MsgServer(int port) : serverSocket(port, DEFAULT_MAX_CONNS), db()
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
                if (err < 0) {
                    return err;
                }
                continue;
            }

            len = atoi(lenStr.c_str());
            err = sock.readNBytes(len, message);
            if (err < 0) {
                //perror("readNBytes");
                LOG("Error during PUT readNBytes: %d.\n", err);
                return err;
            }

            // store parsed message
            db[user].push_back(make_pair(subject, message));

            LOG("Got message.\n\tName: %s\n\tSubject: %s\n\tMessage Length: %d\n\tMessage: '%s'\n", user.c_str(), subject.c_str(), len, message.c_str());
        } else if (verb == "list") {
            string user;
            stream >> user;
            if (user == "") {
                LOG("Missing LIST username\n");
                response = "error - Missing username in LIST request\n";
                err = sock.sendString(response);
                if (err < 0) {
                    return err;
                }
                continue;
            }

            // dump the appropriate DB records
            vector < pair < string,string > > msgList = db[user];
            response = "list ";
            stringstream responseStream(response);
            responseStream << "list ";
            responseStream << msgList.size();
            responseStream << "\n";
            for (vector< pair <string,string> >::iterator msgIterator = msgList.begin();
                 msgIterator != msgList.end();
                 msgIterator++) {
                pair <string, string> msg = (*msgIterator);
                responseStream << distance(msgList.begin(), msgIterator)+1;
                responseStream << " ";
                responseStream << msg.first;
                responseStream << "\n";
            }
            response = (responseStream.str());
        } else if (verb == "get") {
            string user, idxStr;
            int index;

            stream >> user;
            stream >> idxStr;

            if (user == "" || idxStr == "") {
                LOG("Missing GET field\n");
                response = "error - Missing field in GET request\n";
                err = sock.sendString(response);
                if (err < 0) {
                    return err;
                }
                continue;
            }

            index = atoi(idxStr.c_str());
            if (index < 1 || db[user].size() < index) {
                LOG("Out of bounds condition!\n");
                response = "error - GET index is out of bounds\n";
                err = sock.sendString(response);
                if (err < 0) {
                    return err;
                }
                continue;
            }
            // this protocol is apparently 1-indexed :(
            index--;

            // fetch
            //pair<string,string> msg = db[user][index];

            response = "";
            stringstream resStream(response);
            resStream << "message ";
            resStream << db[user][index].first;
            resStream << " ";
            resStream << db[user][index].second.length();
            resStream << "\n";
            resStream << db[user][index].second;
            response = (resStream.str());
        } else if (verb == "reset") {
            db.clear();
        } else {
            response = "error - Verb not found: '";
            response += verb;
            response += "'\n";
        }
        err = sock.sendString(response);
        if (err <= 0) {
            return err;
        }
    }
}

