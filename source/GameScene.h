#pragma once
#include "Mesh.h"
#include "nvrhi/nvrhi.h"
#include <vector>
class GameScene {
	std::vector<Mesh> mesh;
	nvrhi::DeviceHandle mDevice;
public:
	GameScene(nvrhi::DeviceHandle device);
	~GameScene();

	GameScene(const GameScene&) = delete;
	GameScene& operator=(const GameScene&) = delete;
	GameScene(GameScene&&) noexcept = default;
	GameScene& operator=(GameScene&&) noexcept = default;

	void loadStaticModel(const char* path);
};