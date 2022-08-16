#pragma once
#include "../Graphics/Graphics.h"
#include <DirectXMath.h>
namespace TheAftermath {
	struct SceneDesc {
		GraphicsDevice* pDevice;
	};

	class Scene {
	public:
		virtual void Update() = 0;
		//virtual void LoadStaticModel(const wchar_t* path,const DirectX::SimpleMath::Matrix &model) = 0;
		virtual void SetSkyLight(const DirectX::XMFLOAT4& light) = 0;
		//virtual void SetMVP(const DirectX::SimpleMath::Matrix& mvp) = 0;
	};

	Scene* CreateScene(SceneDesc* pDesc);
	void RemoveScene(Scene* pScene);
}