#pragma once
#include "AObject.h"
#include "Graphics.h"
#include <cstdint>
namespace TheAftermath {
	struct SceneDesc {
		Device* pDevice;
		uint32_t mWidth;
		uint32_t mHeight;
	};

	class Scene : public AObject {
	public:
		virtual void Update() = 0;
	};

	Scene* CreateScene(SceneDesc* pDesc);
}