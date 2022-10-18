#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/material.h"
#include "json.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <filesystem>

struct Vertex {
	float Position[3]{};
	float Normal[3]{};
	float TexCoords[2]{};
};

struct Texture {
	std::string FileName;
	float BaseColorFactor[3]{};
	float MetallicRoughnessFactor[2]{};
	float NormalFactor[3]{};
};

struct Mesh {
	std::vector<Vertex> Vertices;
	std::vector<uint32_t> Indices;
	Texture BaseColor;
	Texture MetallicRoughness;
	Texture Normal;
};


void process(std::vector<Mesh> &meshes,const aiScene* scene) {
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
	}

}

int main(int argc, char** argv) {
	std::filesystem::path outputPath{ argv[1] };
	auto outputPathFilename = outputPath.filename();
	std::filesystem::create_directory(outputPathFilename);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(argv[1], aiProcess_Triangulate | aiProcess_GenNormals);
	std::vector<Mesh> meshes;
	process(meshes, scene);

	nlohmann::json gltfData;
	gltfData["asset"] = {
		{ "version" , "2.0" },
		{ "generator" , "TheAftermath" }
	};
	gltfData["scene"] = 0;
	std::cout << gltfData.dump();

	return 0;
}