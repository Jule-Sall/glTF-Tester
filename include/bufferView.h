#ifndef BUFFERVIEW_H
#define BUFFERVIEW_H

#include <unordered_map>


// A view into a buffer generally representing a subset of the buffer
struct BufferView {
	unsigned int buffer;    // The index of the buffer
	size_t byteOffset;      // The offset into the buffer in bytes
	size_t byteLength;      // The length of the buffer in bytes
	size_t byteStride;      // The stride in bytes
	GLenum target;   // The hint representing the intended GPU buffer to use with this buffer view
};
#endif