
#ifndef DEF_PROTO_H
#define DEF_PROTO_H

#include <string>

using namespace std;

enum ReqType { PUT, LIST, GET, RESET, BAD_REQ };

typedef struct Request {
    ReqType type;
    string name;
    int index;
    string subject;
    string message;
    bool valid;
} request;

#endif
