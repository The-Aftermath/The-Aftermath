#include "Scene.h"

namespace TheAftermath {
	class AScene : public Scene {
	public:
		AScene(SceneDesc* pDesc) {
			pDevice = pDesc->pDevice;


		}

		void LoadModel(const std::wstring_view view) {

		}

		ID3D12Device* pDevice;
	};

	Scene* CreateScene(SceneDesc* pDesc) {
		return new AScene(pDesc);
	}
	void RemoveScene(Scene* pScene) {
		auto temp_ptr = dynamic_cast<AScene*>(pScene);
		delete temp_ptr;
	}
}