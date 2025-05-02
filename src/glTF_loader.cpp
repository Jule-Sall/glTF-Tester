#include "../include/glTF_loader.h"


// A map of GPU buffer types and their respective key
std::unordered_map<unsigned int, GLenum> BufferTargets = {
	{34962, GL_ARRAY_BUFFER},
	{34963, GL_ELEMENT_ARRAY_BUFFER}
};


// A Map of component types along with their respective keys
std::unordered_map<unsigned int,GLenum> ComponentTypes = {
	{5120, GL_BYTE},
	{5122, GL_SHORT},
	{5123, GL_UNSIGNED_SHORT},
	{5125, GL_UNSIGNED_INT},
	{5126, GL_FLOAT}
};

glTFloader::glTFloader(const std::string& modelPath, const std::string& directory)
{
	try {
		// Open file
		std::ifstream file(modelPath);
		if (!file.is_open()) {
			std::cout << "Failed to open file at " << modelPath;
		}

		// Parse file
		json JSON = json::parse(file);

		// Close file
		file.close();

		if (!JSON.is_null()) {
			// Load accessors
			if (JSON.contains("accessors")) {
				loadAccessors(JSON["accessors"]);
			}
			// Load buffer views
			if (JSON.contains("bufferViews")) {
				loadBufferViews(JSON["bufferViews"]);
			}
			// Load buffers
			if (JSON.contains("buffers")) {
				loadBuffers(JSON["buffers"]);
				// Load binary geometry
				for (const auto& buffer : Buffers) {
					std::ifstream binFile(directory + buffer.second.uri, std::ios::binary);
					if (binFile.is_open()) {
						loadBinaryGeometry(binFile, buffer.first);
						binFile.close();
					}
					else {
						std::cout << "failed to open file";
					}
				}
			}
		}

	}
	catch (json::exception e) {
		std::cout << e.what();
	}
	catch (std::out_of_range e) {
		std::cout << e.what();
	}
}

std::vector<unsigned char> glTFloader::GetData(Accessor& accessor)
{
	// Get the buffer view of the given accessor
	unsigned int bufferViewIndex = accessor.bufferView;
	BufferView bufferView = BufferViews[bufferViewIndex];

	// Get the buffer of the buffer view
	unsigned int bufferIndex = bufferView.buffer;


	// Get the buffer data
    std::vector<unsigned char> data;
	for (int i = bufferView.byteOffset; i != (bufferView.byteLength + bufferView.byteOffset); ++i) {
		data.push_back(binaryGeometry[bufferIndex][i]);
	}
	
	return data;

}

void glTFloader::loadAccessors(const json& jAccessors)
{
	unsigned int key = 0;
	for (const auto& jAccessor : jAccessors) {
		Accessor accessor;
		
		if (jAccessor.contains("bufferView")) {
			accessor.bufferView = jAccessor["bufferView"];
		}

		if (jAccessor.contains("byteOffset")) {
			accessor.byteOffset = jAccessor["byteOffset"];
		}

		if (jAccessor.contains("componentType")) {
			accessor.componentType = ComponentTypes[jAccessor["componentType"]];
		}

		if (jAccessor.contains("normalized")) {
			accessor.normalized = jAccessor["normalized"];
		}

		if (jAccessor.contains("count")) {
			accessor.count = jAccessor["count"];
		}

		if (jAccessor.contains("type")) {
			accessor.type = jAccessor["type"];
		}

		if (jAccessor.contains("max")) {
			for (int i = 0; i != jAccessor["max"].size(); ++i) {
				accessor.max.push_back(
					jAccessor["max"][i]
				);
			}
		}

		if (jAccessor.contains("min")) {
			for (int i = 0; i != jAccessor["min"].size(); ++i) {
				accessor.min.push_back(
					jAccessor["min"][i]
				);
			}
		}

		Accessors[key++] = accessor;
	}
}

void glTFloader::loadBufferViews(const json& jBufferViews) {
	unsigned int key = 0;
	for (const auto& jBufferView : jBufferViews) {
		BufferView bufferView;

		if (jBufferView.contains("buffer")) {
			bufferView.buffer = jBufferView["buffer"];
		}

		if (jBufferView.contains("byteOffset")) {
			bufferView.byteOffset = jBufferView["byteOffset"];
		}

		if (jBufferView.contains("byteLength")) {
			bufferView.byteLength = jBufferView["byteLength"];
		}

		if (jBufferView.contains("byteStride")) {
			bufferView.byteStride = jBufferView["byteStride"];
		}

		if (jBufferView.contains("target")) {
			bufferView.target = BufferTargets[jBufferView["target"]];
		}

		BufferViews[key++] = bufferView;
	}
}

void glTFloader::loadBuffers(const json& jBuffers) {
	unsigned int key = 0;
	for (const auto& jBuffer : jBuffers) {
		Buffer buffer;

		if (jBuffer.contains("uri")) {
			buffer.uri = jBuffer["uri"];
		}

		if (jBuffer.contains("byteLength")) {
			buffer.byteLength = jBuffer["byteLength"];
		}

		Buffers[key++] = buffer;
	}
}

void glTFloader::loadBinaryGeometry(std::ifstream& binFile, unsigned int buffer)
{
	std::vector<unsigned char> data;

	binFile.seekg(0, std::ios::end);

	std::streamsize size = binFile.tellg();

	binFile.seekg(0, std::ios::beg);

	data.resize(size);

	binFile.read(reinterpret_cast<char*>(data.data()), size);

	binaryGeometry[buffer] = data;

	std::cout << binaryGeometry.size();
}
