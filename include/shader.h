#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <iostream>
#include <fstream>
#include <sstream>

class Shader {
public:
	// Constructor / Destructor
	Shader(const char* fragmentPath, const char* vertexPath);
	~Shader();

	void Use();

private:
	// Program ID
	GLuint ID;
	// Compile shaders and link them to the program
	void compile(const char* vertexCode, const char* fragmentCode);
	// Check for compilation errors
	void checkCompileErr(unsigned int shader, GLenum type);
	// Check for linking errors
	void checkLinkErr(unsigned int program);
};

#endif