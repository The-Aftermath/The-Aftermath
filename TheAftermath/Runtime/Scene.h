#pragma once
#include "d3d12.h"
#include <string_view>
namespace TheAftermath {
	struct SceneDesc {
		ID3D12Device* pDevice;
	};

	class Scene {
	public:
		virtual void LoadModel(const std::wstring_view view) = 0;
	};

	Scene* CreateScene(SceneDesc* pDesc);
	void RemoveScene(Scene* pScene);
}