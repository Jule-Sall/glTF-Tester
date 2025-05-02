#ifndef ACCESSOR_H
#define ACCESSOR_H


#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <unordered_map>


// A typed view into a buffer view that contains raw binary data
struct Accessor {
	unsigned int bufferView; // The index of the bufferView
	size_t   byteOffset;     // The offset relative to the start of the buffer view in bytes
	GLenum componentType;  // The data type of the accessor's components
	bool normalized;         // Specifies whether integer data values are normalized before usage 
	size_t count;            // The number of elements referenced by this accessor 
	std::string type;        // Specifies if the accessor's elements are scalars, vectors or matrices
	std::vector<float> max;  // Maximum value of each component in this accessor
	std::vector<float> min;  // Minimum value of each component in this accessor
};


#endif