#include <iostream>

#include "../include/glTF_loader.h"
#include "../include/shader.h"
#include "../include/camera.h"

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Process input
void processInput(GLFWwindow* window);
// Callbacks 
void framebuffer_size_callback(GLFWwindow* widow, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

size_t getNumComponents(const std::string& type) {
	if (type == "VEC2")
		return 2;
	if (type == "VEC3")
		return 3;
	if (type == "VEC4")
		return 4;
}


size_t getComponentTypeSize(GLenum componentType) {
	switch (componentType) {
	case GL_BYTE:           return sizeof(int8_t);
	case GL_UNSIGNED_BYTE:  return sizeof(uint8_t);
	case GL_SHORT:          return sizeof(int16_t);
	case GL_UNSIGNED_SHORT: return sizeof(uint16_t);
	case GL_INT:            return sizeof(int32_t);
	case GL_UNSIGNED_INT:   return sizeof(uint32_t);
	case GL_FLOAT:          return sizeof(float);
	default:
		std::cerr << "Unsupported component type: " << componentType << std::endl;
		return 0;
	}
}

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
};
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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gladLoadGL();

	// Configure OpenGL state
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	// Create a shader
	Shader shader("resources/shaders/box.vs", "resources/shaders/box.fs");

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
	
	std::vector<Vertex> vertices;
	std::vector<unsigned char> indices;

	GLenum indexType;
	unsigned int index = 0;
	for (auto& mesh : loader.Meshes) {
		// Primitives
		auto primitives = mesh.second.primitives;
		if (primitives[index].attributes.count(POSITION) 
			&& 
			primitives[index].attributes.count(NORMAL))
		{

			
			// Get the position accessor index
			unsigned int posAccessorIndex = primitives[index].attributes[POSITION];
			// Get the position accessor
			Accessor posAccessor = loader.Accessors[posAccessorIndex];
			// Get the normal accessor index
			unsigned int normAccessorIndex = primitives[index].attributes[NORMAL];
			// Get the normal accessor
			Accessor normAccessor = loader.Accessors[normAccessorIndex];

			// Get positions data
			std::vector<unsigned char> positionData = loader.GetData(posAccessor);
			const float* positions = reinterpret_cast<const float*>(positionData.data());
			// Get normals data
			std::vector<unsigned char> normals = loader.GetData(normAccessor);
			const float* norm = reinterpret_cast<const float*>(normals.data());

			unsigned int posIndex = 0;
			unsigned int normIndex = 0;
			for (int i = 0; i != posAccessor.count; ++i) {
				Vertex vertex;
				// 1.Position
				vertex.Position.x = positions[i * 3 + 0];
				vertex.Position.y = positions[i * 3 + 1];
				vertex.Position.z = positions[i * 3 + 2];
				// 2.Normal
				vertex.Normal.x = norm[i * 3 + 0];
				vertex.Normal.y = norm[i * 3 + 1];
				vertex.Normal.z = norm[i * 3 + 2];
				vertices.push_back(vertex);
			}
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			glEnableVertexAttribArray(0);
		    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
			glEnableVertexAttribArray(1);
		}
	    // Indices
		unsigned int indicesAccessorIndex = primitives[index].indices;
		Accessor indicesAccessor = loader.Accessors[indicesAccessorIndex];
		indices = loader.GetData(indicesAccessor);
		indexType = indicesAccessor.componentType;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * getComponentTypeSize(indexType), indices.data(), GL_STATIC_DRAW);
	    index++;
		vertices.clear();
	}

	// Unbind our VAO
	glBindVertexArray(0);
	
	while (!glfwWindowShouldClose(window)) {
		// Per-frame time logic
	   // --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Clear color and buffer screen
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Input
		processInput(window);
		glfwPollEvents();

		// Transformations
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);

		// Use shader program
		shader.Use();

		// Use uniforms to apply transformations
		shader.SetMatrix4f("model", model);
		shader.SetMatrix4f("view", view);
		shader.SetMatrix4f("projection", projection);

		// Bind our VAO
		glBindVertexArray(VAO);
		
		// Draw a box
		glDrawElements(GL_TRIANGLES, indices.size(), indexType, 0);
	

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

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
