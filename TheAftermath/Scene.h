#pragma once
#include "AObject.h"
#include "Device.h"

namespace TheAftermath {

	struct SceneDesc {
		Device* pDevice = nullptr;
	};

	struct Scene : public AObject {
		virtual void Update() = 0;
		virtual void LoadModel(const wchar_t *modelFilePath) = 0;
	};

	Scene* CreateScene(SceneDesc* pDecs);
}
