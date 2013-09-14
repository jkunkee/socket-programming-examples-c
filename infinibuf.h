
#include <sys/types.h>

#define DEFAULT_BUF_SIZE  1024
#define DEFAULT_BUF_COUNT 2

typedef struct c_buf {
    char* buf;
    ssize_t size;
} c_buf;

// This class is neither thread-safe nor reentrant.
class InfiniBuffer {
public:
	// Constructor/Destructor sets
	InfiniBuffer();
	// bufSize is the size of each buffer, bufCount is how many buffers to start with
	InfiniBuffer(int bufSize, int bufCount);
	~InfiniBuffer();

	// other functions
    int maxSize();
    c_buf* getBuffer();
    void returnBuffer(c_buf* cbuf, int bytesUsed);
private:
	char **buffers;
	int curBufIdx;
	int curBufLevel;
	int bufCount;
	int bufSize;
};
