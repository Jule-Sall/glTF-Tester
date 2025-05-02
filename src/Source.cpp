#include <iostream>

#include "../include/glTF_loader.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexSource = 
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main(){\n"
		"gl_Position = vec4(aPos, 1.0f);\n"
		"}\0";

const char* fragmentSource =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main(){\n"
		"FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
		"}\0";

size_t getNumComponents(const std::string& type) {
	if (type == "VEC2")
		return 2;
	if (type == "VEC3")
		return 3;
	if (type == "VEC4")
		return 4;
}
int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
	if (window == nullptr) {
		std::cout << "Failed to create a window";
		return -1;
	}
	
	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	// Compile shaders
	// 1.vertex
	unsigned int vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexSource, NULL);
	glCompileShader(vertex);
	// Check for compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED " << infoLog << std::endl;
	}
	// 2.fragment
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentSource, NULL);
	glCompileShader(fragment);
	// Check for compile errors
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED " << infoLog << std::endl;
	}

	// Link shaders to program
	unsigned int program;
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	// Check for linking errors
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::LINKING_FAILED " << infoLog << std::endl;
	}
	// Delete shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	const std::string modelPath = "resources/models/Triangle/glTF/Triangle.gltf";
	const std::string directory = "resources/models/Triangle/glTF/";
	glTFloader loader(modelPath, directory);

	// VAO / VBO / EBO configuration
	unsigned int VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	for (auto& accessor : loader.Accessors) {
		BufferView bufferView                 = loader.BufferViews[accessor.second.bufferView];
		std::vector<unsigned char> bufferData = loader.GetData(accessor.second);
		if (bufferView.target == GL_ARRAY_BUFFER) {
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(float), bufferData.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(
				0,
				3,
				GL_FLOAT,
				GL_FALSE,
				3 * sizeof(accessor.second.componentType),
				(void*)accessor.second.byteOffset
			);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else if (bufferView.target == GL_ELEMENT_ARRAY_BUFFER) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferData.size() * sizeof(accessor.second.componentType), bufferData.data(), GL_STATIC_DRAW);
		}
    }
	
	glBindVertexArray(0);
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glUseProgram(program);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
	}
	// De-allocate resources
	glDeleteProgram(program);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
