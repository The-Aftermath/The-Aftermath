#pragma once
#include "GameScene.h"
#include "Device.h"

class GameRenderer {
	Device* pDevice = nullptr;
	GameScene* pScene = nullptr;
	nvrhi::DeviceHandle mHandle;

	void _getCullingData() {

	}

	void _drawScene() {

	}
public:

	void init(HWND hwnd, UINT w, UINT h) {
		pDevice = new Device(hwnd, w, h);
		pScene = new GameScene(pDevice->getDeviceHandle());
		mHandle = pDevice->getDeviceHandle();
	}

	void update() {
		_drawScene();
		pDevice->present();
	}

	void destroy() {
		delete pScene;
		delete pDevice;
	}

};