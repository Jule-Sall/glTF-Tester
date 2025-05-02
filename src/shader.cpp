#include "../include/shader.h"

Shader::Shader(const char* fragmentPath, const char* vertexPath)
{
	std::ifstream vShaderFile, fShaderFile;
	// Make sure the files can throw exceptions
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		// Open files
		vShaderFile.open(fragmentPath);
		fShaderFile.open(vertexPath);

		// Read files content into streams
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		// Close files
		vShaderFile.close();
		fShaderFile.close();

		// Read streams into strings
		const std::string vertexCode = vShaderStream.str();
		const std::string fragmentCode = fShaderStream.str();

		// Compile shaders code
		compile(vertexCode.c_str(), fragmentCode.c_str());
	}
	catch (std::ifstream::failure e) {
		std::cout << e.what();
	}
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

void Shader::Use()
{
	glUseProgram(ID);
}

void Shader::compile(const char* vertexCode, const char* fragmentCode)
{
	// 1.Compile vertex shader
	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexCode, NULL);
	glCompileShader(vertex);
	checkCompileErr(vertex, GL_VERTEX_SHADER);

	// 2.Compile fragment shader
	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentCode, NULL);
	glCompileShader(fragment);
	checkCompileErr(fragment, GL_FRAGMENT_SHADER);

	// Link shaders to program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkLinkErr(ID);

	// Delete shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::checkCompileErr(unsigned int shader, GLenum type)
{
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::" << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "COMPILATION_FAILED "
			<< infoLog << std::endl;
	}
}

void Shader::checkLinkErr(unsigned int program)
{
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::LINKING_FAILED " << infoLog << std::endl;
	}
}
