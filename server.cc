#include "server.h"

Server::Server(int port) {
    // setup variables
    port_ = port;

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
    InfiniBuffer mybuf;
    string s = mybuf.GetNBytes(3);
    printf("nbytes: '%s'\n", s.c_str());

    // setup client
    int client;
    struct sockaddr_in client_addr;
    socklen_t clientlen = sizeof(client_addr);

      // accept clients
    while ((client = accept(server_,(struct sockaddr *)&client_addr,&clientlen)) > 0) {

        handle(client);
        close(client);
    }

    close(server_);

}

void
Server::handle(int client) {
    // loop to handle all requests
    while (1) {
        // get a request
        Request request = collect_request(client);
        // break if client is done or an error occurred
        if (request.type == BAD_REQ)
            break;
        // send response
        bool success = send_response(client,"Hi there. You said something.");
        // break if an error occurred
        if (not success)
            break;
    }
}

Request
Server::collect_request(int client) {
    Request req;
    req.type = BAD_REQ;
    string rawReq;

    // read until we get a newline
    while ((rawReq = buf_.ScanForSentinel('\n')) == "") {
        // make a buffer (inefficient, I know; v2 will find a buffer)
        c_buf *buf = buf_.GetBuffer();
        // recv off the socket
        int nread = recv(client,buf->buf,buf->size,0);
        if (nread < 0) {
            if (errno == EINTR) {
                // the socket call was interrupted -- try again
                continue;
            } else {
                perror("while scanning for sentinel");
                // an error occurred, so break out
                return req;
            }
        } else if (nread == 0) {
            // the socket is closed
            return req;
        }
        buf_.ReturnBuffer(buf, nread);
    }
    switch (rawReq.at(0)) {
    }
    return req;
}

bool
Server::send_response(int client, string response) {
    // prepare to send response
    const char* ptr = response.c_str();
    int nleft = response.length();
    int nwritten;
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
    return true;
}
