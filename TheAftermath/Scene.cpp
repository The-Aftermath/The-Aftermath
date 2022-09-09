#include "Scene.h"

namespace TheAftermath {
	struct AScene : public Scene {
		AScene(SceneDesc* pDesc) {
			pDevice = pDesc->pDevice;
		}

		~AScene() {

		}

		void Release() {
			delete this;
		}

		void Update() {

			pDevice->Present();
		}

		Device* pDevice;
	};

	Scene* CreateScene(SceneDesc* pDesc) {
		return new AScene(pDesc);
	}
}