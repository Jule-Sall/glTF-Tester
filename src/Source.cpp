#include "../include/glTF_loader.h"


int main() {
	glTFloader loader("resources/models/Triangle/glTF/Triangle.gltf", "resources/models/Triangle/glTF/");
	std::cout << loader.Accessors.size();
	return 0;
}



