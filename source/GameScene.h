#pragma once
#include "Mesh.h"
#include "AABB.h"
#include "nvrhi/nvrhi.h"
#include <vector>
class GameScene {
	std::vector<Mesh> mMesh;

	DirectX::XMFLOAT4X4 mCameraView;
	DirectX::XMFLOAT4X4 mCameraProj;
public:
	GameScene(nvrhi::DeviceHandle device);
	~GameScene();
};