#pragma once
#include "../Graphics/Graphics.h"
#include <DirectXMath.h>
namespace TheAftermath {
	struct SceneDesc {
		GraphicsDevice* pDevice;
		DirectX::XMFLOAT4 mLight;
	};

	class Scene {
	public:
		virtual void Update() = 0;
		//virtual void LoadStaticModel(const wchar_t* path,const DirectX::SimpleMath::Matrix &model) = 0;
	};

	Scene* CreateScene(SceneDesc* pDesc);
	void RemoveScene(Scene* pScene);
}