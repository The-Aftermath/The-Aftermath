#include <assimp/Importer.hpp>
#define CGLTF_IMPLEMENTATION
#define CGLTF_WRITE_IMPLEMENTATION
#include <cgltf_write.h>


int main(int argc, char** argv) {
	Assimp::Importer importer;
	cgltf_options options{};
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse_file(&options, "C:\\Users\\42937\\Documents\\GitHub\\glTF-Sample-Models\\2.0\\Box With Spaces\\glTF\\Box With Spaces.gltf", &data);

	cgltf_free(data);
	//cgltf_free(pddd);
	return 0;
}