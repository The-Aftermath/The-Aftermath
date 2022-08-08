#pragma once
#include "d3d12.h"
namespace TheAftermath {
	struct SceneDesc {
		ID3D12Device* pDevice;
	};

	class Scene {
	public:

	};

	Scene* CreateScene(SceneDesc* pDesc);
	void RemoveScene(Scene* pScene);
}