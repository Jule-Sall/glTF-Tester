#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>

#include <string>
#include <map>

enum Attribute {
	NORMAL,
	POSITION,
	TEXCOORD_0
};

// Geometry to be rendered within the given material
struct Mesh_Primitive {
	std::map<Attribute, unsigned int> attributes; // A collection of pairs where each key corresponds to a mesh attribute semantic and each value is the index of the accessor containing attribute's data
	unsigned int  indices = 0; // The index of the accessor that contains the vertex indices
	unsigned int material; // The index of the material to apply to this primitive when rendering
	GLenum mode;           // Type of primitive to render
};

// A set of primitives to be rendered
struct Mesh {
	std::vector<Mesh_Primitive> primitives; // A collection of primitives, each defining geometry to be rendered
    
};
#endif