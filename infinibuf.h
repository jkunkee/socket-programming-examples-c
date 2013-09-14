
#ifndef INC_INFINIBUF_H
#define INC_INFINIBUF_H

#include <sys/types.h>
#include <string>

using namespace std;

typedef struct c_buf {
    char* buf;
    ssize_t size;
} c_buf;

// This class is neither thread-safe nor reentrancy-safe.
class InfiniBuffer {
public:
	// Constructor/Destructor sets
	InfiniBuffer();
	~InfiniBuffer();

	// Consumer functions
	string ScanForSentinel(char sentinel);
	string GetNBytes(int n);

    // Producer functions
    c_buf* GetBuffer();
    void ReturnBuffer(c_buf* buf, int used);
private:
    string buffer;
};

#endif
