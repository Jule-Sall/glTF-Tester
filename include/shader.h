#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>

class Shader {
public:
	// Constructor / Destructor
	Shader(const char* fragmentPath, const char* vertexPath);
	~Shader();

	void Use();

	// Uniforms
	void SetMatrix4f(const std::string& name, glm::mat4 mat) {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

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