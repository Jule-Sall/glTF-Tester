#include <iostream>

#include "stb_image.h"

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

// Light position
glm::vec3 lightPos = glm::vec3(-0.6f, 4.0f, 1.0f);

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
	glm::vec3 Color;
	glm::vec2 TexCoord;
};

std::vector<unsigned int> VAOs;
std::vector<unsigned int> VBOs;
std::vector<unsigned int> EBOs;
std::unordered_map<unsigned int, std::vector<unsigned char>> indices;
std::unordered_map<unsigned int, GLenum> modes;
std::vector<unsigned int> Textures;
std::vector<size_t> vertices_count;

void Draw(Shader& shader);
void setUpMesh(Mesh& mesh, glTFloader& loader);
void ProcessMesh(glTFloader& loader);

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
	Shader shader("resources/shaders/textured_cube.vs", "resources/shaders/textured_cube.fs");

	const std::string modelPath = "resources/models/BoxTextured/glTF/BoxTextured.gltf";
	const std::string directory = "resources/models/BoxTextured/glTF/";
	glTFloader loader(modelPath, directory);
	ProcessMesh(loader);

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
		
		Draw(shader);


		glfwSwapBuffers(window);
	}
	// De-allocate resources
	for (unsigned int& VBO : VBOs) {
		glDeleteBuffers(1, &VBO);
	}
	VBOs.clear();
	for (unsigned int& EBO : EBOs) {
		glDeleteBuffers(1, &EBO);
	}
	EBOs.clear();
	for (unsigned int& VAO : VAOs) {
		glDeleteVertexArrays(1, &VAO);
	}
	VAOs.clear();
	for (unsigned int& texture : Textures) {
		glDeleteTextures(1, &texture);
	}
	Textures.clear();

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

void Draw(Shader& shader) {
	shader.Use();
	for (int i = 0; i != VAOs.size(); ++i) {
		glBindVertexArray(VAOs[i]);
		glBindTexture(GL_TEXTURE_2D, Textures[i]);
		if (!indices.empty()) {
			glDrawElements(modes[i], indices[i].size(), GL_UNSIGNED_SHORT, 0);
		}
		else {
			glDrawArrays(modes[i], 0, vertices_count[i]);
		}
		glBindVertexArray(0);
	}
}


void setUpMesh(Mesh& mesh, glTFloader& loader) {
	unsigned int i = 0;
	for (auto& primitive : mesh.primitives) {
		VAOs.resize(i + 1);
		VBOs.resize(i + 1);
		EBOs.resize(i + 1);

		glGenVertexArrays(1, &VAOs[i]);
		glBindVertexArray(VAOs[i]);

		glGenBuffers(1, &VBOs[i]);
		glGenBuffers(1, &EBOs[i]);

		std::vector<float> positions;
		std::vector<float> normals;
		std::vector<float> colors;
		std::vector<float> texCoords;
		std::vector<Vertex> vertices;
		unsigned int numVertices;

		if (primitive.attributes.count(POSITION)) {
			Accessor posAccessor = loader.Accessors[primitive.attributes[POSITION]];
			numVertices = posAccessor.count;
			std::vector<unsigned char> posData = loader.GetData(posAccessor);
			size_t elementCount = posAccessor.count * getNumComponents(posAccessor.type);
			positions.resize(elementCount);
			memcpy(positions.data(), posData.data(), elementCount * sizeof(float));
		}
		if (primitive.attributes.count(NORMAL)) {
			Accessor normAccessor = loader.Accessors[primitive.attributes[NORMAL]];
			std::vector<unsigned char> normData = loader.GetData(normAccessor);
			size_t elementCount = normAccessor.count * getNumComponents(normAccessor.type);
			normals.resize(elementCount);
			memcpy(normals.data(), normData.data(), elementCount * getComponentTypeSize(normAccessor.componentType));
		}
		if (primitive.attributes.count(TEXCOORD_0)) {
			Accessor texCoordAccessor = loader.Accessors[primitive.attributes[TEXCOORD_0]];
			std::vector<unsigned char> texCoordData = loader.GetData(texCoordAccessor);
			size_t elementCount = texCoordAccessor.count * getNumComponents(texCoordAccessor.type);
			texCoords.resize(elementCount);
			memcpy(texCoords.data(), texCoordData.data(), elementCount * getComponentTypeSize(texCoordAccessor.componentType));
		}
		if (primitive.attributes.count(COLOR_0)) {
			Accessor colorAccessor = loader.Accessors[primitive.attributes[COLOR_0]];
			std::vector<unsigned char> colorData = loader.GetData(colorAccessor);
			size_t elementCount = colorAccessor.count * getNumComponents(colorAccessor.type);
			colors.resize(elementCount);
			memcpy(colors.data(), colorData.data(), elementCount * getComponentTypeSize(colorAccessor.componentType));
		}
		for (int j = 0; j != numVertices; ++j) {
			Vertex vertex;
			if (!positions.empty()) {
				vertex.Position.x = positions[j * 3 + 0];
				vertex.Position.y = positions[j * 3 + 1];
				vertex.Position.z = positions[j * 3 + 2];
			}
			if (!normals.empty()) {
				vertex.Normal.x = normals[j * 3 + 0];
				vertex.Normal.y = normals[j * 3 + 1];
				vertex.Normal.z = normals[j * 3 + 2];
			}
			if (!colors.empty()) {
				vertex.Color.x = colors[j * 3 + 0];
				vertex.Color.y = colors[j * 3 + 1];
				vertex.Color.z = colors[j * 3 + 2];
			}
			if (!texCoords.empty()) {
				vertex.TexCoord.x = texCoords[j * 2 + 0];
				vertex.TexCoord.y = texCoords[j * 2 + 1];
			}
			vertices.push_back(vertex);
		}
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	    // Indices
		if (primitive.indices) {
			Accessor indicesAccessor = loader.Accessors[*(primitive.indices)];
			indices[i] = loader.GetData(indicesAccessor);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * getComponentTypeSize(indicesAccessor.componentType), indices[i].data(), GL_STATIC_DRAW);
		}
		// Primitive's type
		modes[i] = primitive.mode;

		// Texture set up
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		if (primitive.material) {
			unsigned int material = *(primitive.material);
			Image image = loader.Images[loader.Textures[material].source];
			Sampler sampler = loader.Samplers[loader.Textures[material].sampler];
			int width, height, nrChannels;
			unsigned char* data = stbi_load(image.uri.c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.magFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);

				stbi_image_free(data);
			}
			else {
				std::cout << "failed to load texture" << std::endl;
			}
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		Textures.push_back(texture);
		vertices_count.push_back(vertices.size());
		i++;
	}
}
void ProcessMesh(glTFloader& loader) {
	for (auto& mesh : loader.Meshes) {
		setUpMesh(mesh.second, loader);
	}
}
