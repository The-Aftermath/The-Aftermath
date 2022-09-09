#pragma once
#include "AObject.h"
#include "Graphics.h"
#include <cstdint>
namespace TheAftermath {
	struct SceneDesc {
		Device* pDevice = nullptr;
		uint32_t mWidth = 800;
		uint32_t mHeight = 600;
	};

	class Scene : public AObject {
	public:
		virtual void Update() = 0;
	};

	Scene* CreateScene(SceneDesc* pDesc);
}