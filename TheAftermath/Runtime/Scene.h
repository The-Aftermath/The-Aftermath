#pragma once
#include "../Graphics/Graphics.h"

namespace TheAftermath {
	struct SceneDesc {
		GraphicsDevice* pDevice;
	};

	class Scene {
	public:
		virtual void Update() = 0;
	};

	Scene* CreateScene(SceneDesc* pDesc);
	void RemoveScene(Scene* pScene);
}