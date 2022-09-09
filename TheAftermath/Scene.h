#pragma once
#include "AObject.h"
#include "Graphics.h"
namespace TheAftermath {
	struct SceneDesc {
		Device* pDevice;
	};

	class Scene : public AObject {
	public:
		virtual void Update() = 0;
	};

	Scene* CreateScene(SceneDesc* pDesc);
}