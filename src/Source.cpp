#include <iostream>

#include "../include/glTF_loader.h"
#include "../include/shader.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Process input
void processInput(GLFWwindow* window);

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

	// Create a shader
	Shader shader("resources/shaders/triangle.vs", "resources/shaders/triangle.fs");

	const std::string modelPath = "resources/models/Box/glTF/Box.gltf";
	const std::string directory = "resources/models/Box/glTF/";
	glTFloader loader(modelPath, directory);

	// VAO / VBO / EBO configuration
	unsigned int VAO, VBO, EBO;

	// Save VBO and EBO configuration state into our VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	std::vector<unsigned char> vertices;
	std::vector<unsigned char> indices;

	// Use accessors to configure VBO/EBO
	for (auto& accessor : loader.Accessors) {
		// Get the current buffer view
		BufferView bufferView    = loader.BufferViews[accessor.second.bufferView];
		// Check if it's an array buffer view
		if (bufferView.target == GL_ARRAY_BUFFER) {
			// Load buffer data into vertices
			vertices = loader.GetData(accessor.second);
			// Bind our VBO object to the array buffer
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			// Copy the vertices into the VBO
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
			// Configure attributes pointer
			glVertexAttribPointer(
				0,
				getNumComponents(accessor.second.type),
				accessor.second.componentType,
				accessor.second.normalized ? GL_FALSE : GL_TRUE,
				getNumComponents(accessor.second.type) * sizeof(accessor.second.componentType),
				(void*)accessor.second.byteOffset
			);
			// Enable vertex attributes
			glEnableVertexAttribArray(0);
			// Unbind VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
	
		}
		// Check if buffer view is an element buffer view
		else if (bufferView.target == GL_ELEMENT_ARRAY_BUFFER) {
			// Load buffer into indices
			indices = loader.GetData(accessor.second);
			// Bind our EBO object to element array buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			// Copy the indices into our EBO object
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(accessor.second.componentType), indices.data(), GL_STATIC_DRAW);
		}
    }
	// Unbind our VAO
	glBindVertexArray(0);
	
	while (!glfwWindowShouldClose(window)) {
		// Input
		processInput(window);
		glfwPollEvents();

		// Use shader program
		shader.Use();
		// Bind our VAO
		glBindVertexArray(VAO);
		// Draw a box
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
		
		glfwSwapBuffers(window);
	}
	// De-allocate resources
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	// Close if user presses the espace key
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
