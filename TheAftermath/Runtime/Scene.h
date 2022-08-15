#pragma once
#include "../Graphics/Graphics.h"
#include "../Math/SimpleMath.h"
namespace TheAftermath {
	struct SceneDesc {
		GraphicsDevice* pDevice;
	};

	class Scene {
	public:
		virtual void Update() = 0;
		virtual void LoadStaticModel(const wchar_t* path,const DirectX::SimpleMath::Matrix &model) = 0;
		virtual void SetSkyLight(const DirectX::SimpleMath::Vector4& light) = 0;
	};

	Scene* CreateScene(SceneDesc* pDesc);
	void RemoveScene(Scene* pScene);
}