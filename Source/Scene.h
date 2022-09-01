#pragma once
#include "AObject.h"
#include "Graphics.h"
namespace TheAftermath {
	struct SceneDesc {
		Device* pDevice = nullptr;
		uint32_t mWidth = 800;
		uint32_t mHeight = 600;
	};

	struct Scene : public AObject {
		virtual void Update() = 0;
	};

	Scene* CreateScene(SceneDesc* pDesc);
}