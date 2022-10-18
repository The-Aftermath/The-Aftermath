#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/material.h"
#include "assimp/GltfMaterial.h"
#include "assimp/types.h"
#include "assimp/mesh.h"
#include "json.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <filesystem>
#include <winrt/Windows.Foundation.h>

using namespace winrt;
using namespace Windows::Foundation;

struct Vertex {
	float Position[3]{};
	float Normal[3]{};
	float TexCoords[2]{};
};

struct Texture {
	std::string FileName;
};

struct Mesh {
	std::vector<Vertex> Vertices;
	std::vector<uint32_t> Indices;
	Texture BaseColor;
	Texture MetallicRoughness;
	Texture Normal;
};


void process(const std::filesystem::path& modelPath, std::vector<Mesh>& meshes, const aiScene* scene) {
	uint32_t meshCount = scene->mNumMeshes;
	for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex) {
		Mesh mesh;
		auto verticesCount = scene->mMeshes[meshIndex]->mNumVertices;
		for (uint32_t verticesIndex = 0; verticesIndex < verticesCount; ++verticesIndex) {
			Vertex vertex;
			vertex.Position[0] = scene->mMeshes[meshIndex]->mVertices[verticesIndex].x;
			vertex.Position[1] = scene->mMeshes[meshIndex]->mVertices[verticesIndex].y;
			vertex.Position[2] = scene->mMeshes[meshIndex]->mVertices[verticesIndex].z;
			vertex.Normal[0] = scene->mMeshes[meshIndex]->mNormals[verticesIndex].x;
			vertex.Normal[1] = scene->mMeshes[meshIndex]->mNormals[verticesIndex].y;
			vertex.Normal[2] = scene->mMeshes[meshIndex]->mNormals[verticesIndex].z;

			if (!scene->mMeshes[meshIndex]->mTextureCoords[0]) {
				vertex.TexCoords[0] = 0.f;
				vertex.TexCoords[1] = 0.f;
			}
			else {
				vertex.TexCoords[0] = scene->mMeshes[meshIndex]->mTextureCoords[0][verticesIndex].x;
				vertex.TexCoords[1] = scene->mMeshes[meshIndex]->mTextureCoords[0][verticesIndex].y;
			}

			mesh.Vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < scene->mMeshes[meshIndex]->mNumFaces; i++)
		{
			aiFace face = scene->mMeshes[meshIndex]->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				mesh.Indices.push_back(face.mIndices[j]);
			}
		}

		aiMaterial* material = scene->mMaterials[scene->mMeshes[meshIndex]->mMaterialIndex];
		auto parent = modelPath.parent_path();
		auto outputPathName = modelPath.stem();
		if (aiString mrTexture; material->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &mrTexture) == aiReturn_SUCCESS) {
			//my_mesh.MetallicRoughness.FileName = mrTexture.C_Str();
			auto realTexPath = parent / mrTexture.C_Str();
			std::filesystem::copy(realTexPath, outputPathName, std::filesystem::copy_options::skip_existing);
		}
		else {
			mrTexture;
		}
	}

}

int main(int argc, char** argv) {

	init_apartment();

	std::filesystem::path modelPath{ "C:\\Users\\42937\\Documents\\GitHub\\glTF-Sample-Models\\2.0\\Box With Spaces\\glTF\\Box With Spaces.gltf" };
	auto outputPathName= modelPath.stem();
	std::filesystem::create_directory(outputPathName);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(modelPath.string(), aiProcess_Triangulate | aiProcess_GenNormals);
	std::vector<Mesh> meshes;
	try {
		process(modelPath, meshes, scene);
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cout << e.what() << std::endl;
	}

	nlohmann::json gltfData;
	gltfData["asset"] = {
		{ "version" , "2.0" },
		{ "generator" , "TheAftermath" }
	};
	gltfData["scene"] = 0;
	std::cout << gltfData.dump();

	return 0;
}