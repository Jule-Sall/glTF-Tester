#ifndef BUFFER_H
#define BUFFER_H

#include <string>

// A buffer points to some binary geometry, animation, or skins
struct Buffer {
	std::string uri;        // The URI of the buffer
	size_t byteLength;      // The length of the buffer in bytes
};

#endif