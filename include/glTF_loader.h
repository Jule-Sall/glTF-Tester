#ifndef GLTF_LOADER_H
#define GLTF_LOADER_H

#include <iostream>
#include <fstream>


#include "nlohmann/json.hpp"

#include "buffer.h"
#include "accessor.h"
#include "bufferView.h"
#include "mesh.h"

using json = nlohmann::json;


class glTFloader {
public:
	// A map of accessors and their respective keys
	std::unordered_map<unsigned int, Accessor> Accessors;
	
	// A map of buffer views and their respective keys
	std::unordered_map<unsigned int, BufferView> BufferViews;
	
	// A map of buffers and their respective keys
	std::unordered_map<unsigned int, Buffer> Buffers;
	
	// A map of meshes and their respective keys
	std::unordered_map<unsigned int, Mesh> Meshes;

	// FOR later
	/*
	// A map of images and their respective keys
	std::unordered_map<unsigned int, Image> Images;
	
	// A map of materials and their respective keys
	std::unordered_map<unsigned int, Material> Materials;
	
	// A map of textures and their respective keys
	std::unordered_map<unsigned int, Texture> Textures;
	
	// A map of samplers and their respective keys
	std::unordered_map<unsigned int, Sampler> Samplers;
	
	
	
	// A map of nodes and their respective keys
	std::unordered_map<unsigned int, Node> Nodes;
	
	// A map of scenes and their respective keys
	std::unordered_map<unsigned int, Scene> Scenes;
	*/

	// Constructor
	glTFloader(const std::string& modelPath, const std::string& directory);

	std::vector<unsigned char> GetData(Accessor& accessor);

private:
	// A binary geometry to store the contents needed for drawing
	std::unordered_map<unsigned int, std::vector<unsigned char>> binaryGeometry;
	
	void loadAccessors(const json& jAccessors);
	void loadBufferViews(const json& jBufferViews);
	void loadBuffers(const json& jBuffers);
	void loadMeshes(const json& jMeshes);
	/* TO-DO
	void loadImages(const json& jImages);
	void loadMaterials(const json& jMaterials);
	void loadTextures(const json& jTextures);
	void loadSamplers(const json& jSamplers);
	void loadNodes(const json& jNodes);
	void loadScenes(const json& jScenes);
	*/
	void loadBinaryGeometry(std::ifstream& binFile, unsigned int buffer);
};

#endif