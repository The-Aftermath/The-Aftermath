#pragma once
#include <cstdint>
#include <vector>
#include <DirectXMath.h>
struct Mesh {
	std::vector<DirectX::XMFLOAT3> Positions;
	std::vector<DirectX::XMFLOAT3> Normals;
	std::vector<DirectX::XMFLOAT2> TexCoords0;
	std::vector<uint32_t> Indices;
};