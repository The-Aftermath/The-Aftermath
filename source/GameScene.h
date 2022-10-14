#pragma once
#include "nvrhi/nvrhi.h"
class GameScene {
public:
	GameScene(nvrhi::DeviceHandle device);
	~GameScene();

	void loadStaticModel();
};