#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <DirectXMath.h>

struct MeshData {
	std::vector<DirectX::XMFLOAT3> Positions;
	std::vector<DirectX::XMFLOAT3> Normals;
	std::vector<DirectX::XMFLOAT2> TexCoords0;
	std::vector<uint16_t> Indices;

	std::string BaseColor;
	std::string Normal;
	std::string MetallicRoughness;
};