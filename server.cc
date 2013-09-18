#include "server.h"

#include <iostream>
#include <sstream>

// for logging
#include "logger.h"

bool debug = false;

Server::Server(int port) {
    Server(port, false);
}

Server::Server(int port, bool debug) {
    // setup variables
    port_ = port;
    debug_ = debug;

    // create and run the server
    create();
    serve();
}

Server::~Server() {
}

void
Server::create() {
    struct sockaddr_in server_addr;

    // setup socket address structure
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // create socket
    server_ = socket(PF_INET,SOCK_STREAM,0);
    if (!server_) {
        perror("socket");
        exit(-1);
    }

    // set socket to immediately reuse port when the application closes
    int reuse = 1;
    if (setsockopt(server_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt");
        exit(-1);
    }

    // call bind to associate the socket with our local address and
    // port
    if (bind(server_,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        perror("bind");
        exit(-1);
    }

      // convert the socket to listen for incoming connections
    if (listen(server_,SOMAXCONN) < 0) {
        perror("listen");
        exit(-1);
    }
}

void
Server::serve() {
    // setup client
    int client;
    struct sockaddr_in client_addr;
    socklen_t clientlen = sizeof(client_addr);

      // accept clients
    while ((client = accept(server_,(struct sockaddr *)&client_addr,&clientlen)) > 0) {
        LOG("Hey, a client!\n");
        handle(client);
        LOG("Client handling finished. Now to close it...\n");
        close(client);
        LOG("That client is CLOSED. :)\n");
    }

    close(server_);

}

void
Server::handle(int client) {
    LOG("Handling client on socket #%d\n", client);
    // loop to handle all requests
    while (1) {
        bool success;
        // get a request
        Request request;
        success = collect_request(client, &request);
        if (not success) {
            break;
        }
        string response = "Boo\n";
        // break if client is done or an error occurred
        if (request.type == BAD_REQ) {
            LOG("got bad request!\n");
            response = "Badly Formatted Request\n";
        }
        // send response
        success = send_response(client, response);
        // break if an error occurred
        if (not success)
            break;
    }
}

bool
Server::collect_request(int client, Request* req) {
    if (req == NULL) {
        return false;
    }
    // the default state of this request is BAD_REQ.
    req->type = BAD_REQ;

    // have a place to store the request while parsing it
    string rawReq;

    LOG("Hi. I'm collecting a request.\n");

    // read until we get a newline
    // ScanForSentinel returns an empty string until it finds the delimiter.
    while ((rawReq = buf_.ScanForSentinel('\n')) == "") {
        // make a buffer (inefficient, I know; v2 will find a buffer or something)
        c_buf *buf = buf_.GetBuffer();
        LOG("Firing up recv\n");
        // recv off the socket
        int nread = recv(client,buf->buf,buf->size,0);
        if (nread < 0) {
            if (errno == EINTR) {
                // the socket call was interrupted -- try again
                continue;
            } else {
                perror("while scanning for sentinel");
                // an error occurred, so break out
                LOG("saw error on recv: %d\n", errno);
                return false;
            }
        } else if (nread == 0) {
            // the socket is closed
            LOG("collect_request recv reports that socket is closed.\n");
            return false;
        }
        buf_.ReturnBuffer(buf, nread);
        LOG("Just read one packet: '%s'\n", rawReq.c_str());
    }

    // if we somehow got an empty string, worry
    LOG("Hey! I have a whole request! '%s'\n", rawReq.c_str());

    // parse the received command
    stringstream stream(rawReq);
    int msgLen;
    string type;

    switch (rawReq.at(0)) {
    case 'p':
        // default delimiter is whitespace
        stream >> type;
        if (type != "put") {
            LOG("Not a put request! :P\n");
            return true;
        }
        stream >> req->name;
        if (req->name == "") {
            LOG("No name found!\n");
            return true;
        }
        stream >> req->subject;
        if (req->subject == "") {
            LOG("No subject found!\n");
            return true;
        }
        stream >> msgLen;

        // now read in the message
        req->message = "";

        while (req->message == "" && msgLen != 0) {
            c_buf* buf = buf_.GetBuffer();
            LOG("Firing up recv for message\n");
            // recv off the socket
            int nread = recv(client,buf->buf,buf->size,0);
            if (nread < 0) {
                if (errno == EINTR) {
                    // the socket call was interrupted -- try again
                    continue;
                } else {
                    perror("while reading n bytes");
                    // an error occurred, so break out
                    LOG("saw error on recv for message: %d\n", errno);
                    return false;
                }
            } else if (nread == 0) {
                // the socket is closed
                LOG("collect_request message recv reports that socket is closed.\n");
                return false;
            }
            buf_.ReturnBuffer(buf, nread);
            req->message = buf_.GetNBytes(msgLen);
        }

        LOG("Got message.\n\tName: %s\n\tSubject: %s\n\tMessage Length: %d\n\tMessage:\n'%s'\n", req->name.c_str(), req->subject.c_str(), msgLen, req->message.c_str());

        req->type = PUT;
        break;
    case 'l':
        req->type = LIST;
        break;
    case 'g':
        req->type = GET;
        break;
    case 'r':
        req->type = RESET;
        break;
    }
    return true;
}

bool
Server::send_response(int client, string response) {
    // prepare to send response
    const char* ptr = response.c_str();
    int nleft = response.length();
    int nwritten;
    LOG("Sending response: '%s'\n", response.c_str());
    // loop to be sure it is all sent
    while (nleft) {
        if ((nwritten = send(client, ptr, nleft, 0)) < 0) {
            if (errno == EINTR) {
                // the socket call was interrupted -- try again
                continue;
            } else {
                // an error occurred, so break out
                perror("write");
                return false;
            }
        } else if (nwritten == 0) {
            // the socket is closed
            return false;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    LOG("response sent.\n");
    return true;
}
