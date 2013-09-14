#include "infinibuf.h"

#define DEFAULT_BUF_SIZE  1024

// constructors
InfiniBuffer::InfiniBuffer() {}
InfiniBuffer::~InfiniBuffer() {}

// other functions
// Consumer functions
string InfiniBuffer::ScanForSentinel(char sentinel) {
    size_t sentinelIdx = buffer.find_first_of(sentinel, 0);
    string retVal = "";

    if (sentinelIdx == string::npos) {
        return retVal;
    }

    return GetNBytes(sentinelIdx+1);
}
string InfiniBuffer::GetNBytes(int n) {
    string retVal = "";

    if (buffer.length() < n) {
        return retVal;
    }

    retVal = buffer.substr(0, n);
    buffer.erase(0, n);

    return retVal;
}

// Producer functions
c_buf* InfiniBuffer::GetBuffer() {
    c_buf* buf = new c_buf;
    buf->buf = new char[DEFAULT_BUF_SIZE];
    buf->size = DEFAULT_BUF_SIZE;
    return buf;
}
void InfiniBuffer::ReturnBuffer(c_buf* buf, int used) {
    if (buf == NULL) {
        return;
    }
    if (used > buf->size) {
        return;
    }

    buffer.append(buf->buf, 0, used);
    delete buf->buf;
    delete buf;
}
