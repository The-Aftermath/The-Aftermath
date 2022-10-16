#pragma once
#include "Mesh.h"
#include <Windows.h>
#include <vector>
#include <DirectXMath.h>

class GameScene {
	std::vector<MeshData> mMesh;
	DirectX::XMFLOAT4X4 m;
	DirectX::XMFLOAT4X4 v;
	DirectX::XMFLOAT4X4 p;
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 focus;
	DirectX::XMFLOAT3 up;
public:
	GameScene(UINT w, UINT h);
	~GameScene();

	GameScene(const GameScene&) = delete;
	GameScene& operator=(const GameScene&) = delete;
	GameScene(GameScene&&) noexcept = default;
	GameScene& operator=(GameScene&&) noexcept = default;

	void dataCulling() {

	}

	void loadStaticModel(const char* path);
};