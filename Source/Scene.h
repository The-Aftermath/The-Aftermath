#pragma once
#include "AObject.h"
#include "Graphics.h"
namespace TheAftermath {
	struct SceneDesc {
		Device* pDevice = nullptr;
	};

	struct Scene : public AObject {
		virtual void Update() = 0;
	};

	Scene* CreateScene(SceneDesc* pDesc);
}