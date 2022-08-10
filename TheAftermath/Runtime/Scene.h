#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string_view>
namespace TheAftermath {
	struct SceneDesc {
		ID3D12Device* pDevice;
		IDXGISwapChain* pSwapChain;
	};

	class Scene {
	public:
		virtual void LoadModel(const std::wstring_view view) = 0;
	};

	Scene* CreateScene(SceneDesc* pDesc);
	void RemoveScene(Scene* pScene);
}