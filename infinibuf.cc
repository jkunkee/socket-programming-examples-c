#include "infinibuf.h"

#define DEFAULT_BUF_SIZE  1024
#define DEFAULT_BUF_COUNT 2

// constructors
InfiniBuffer::InfiniBuffer() {
    InfiniBuffer(DEFAULT_BUF_SIZE, DEFAULT_BUF_COUNT);
}
InfiniBuffer::InfiniBuffer(int _bufSize, int _bufCount) {
    bufCount = _bufCount;
    bufSize = _bufSize;

	curBufIdx = 0;
	curBufLevel = 0;

    buffers = new char*[_bufCount];

    for (int idx = 0; idx < bufCount; idx++) {
        buffers[idx] = new char[_bufSize+1];
    }
}
InfiniBuffer::~InfiniBuffer() {
    for (int idx = 0; idx < bufCount; idx++) {
        delete buffers[idx];
    }
    delete buffers;
}

// other functions
int InfiniBuffer::maxSize() {
    return bufCount*bufSize;
}
c_buf* InfiniBuffer::getBuffer() {
    c_buf* retVal = new c_buf;
    retVal->buf = buffers[curBufIdx]+curBufLevel;
    retVal->size = bufSize - curBufLevel;
    return retVal;
}
void InfiniBuffer::returnBuffer(c_buf* cbuf, int bytesUsed) {
    delete cbuf;
    curBufLevel += bytesUsed;
    if (curBufLevel >= bufSize) {

    }
}
