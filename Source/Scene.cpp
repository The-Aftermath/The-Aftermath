#include "Scene.h"
#include <exception>
namespace TheAftermath {

	struct AScene : public Scene {
		AScene(SceneDesc* pDesc) {
			pDevice = pDesc->pDevice;
			if (!pDevice) {
				throw std::exception("Device is nullptr.");
			}

		}

		~AScene() {}

		void Release() {
			delete this;
		}

		void Update() {
			pDevice->BeginDraw();
			pDevice->EndDraw();
			pDevice->Present();
		}

		Device* pDevice;
	};

	Scene* CreateScene(SceneDesc* pDesc) {
		return new AScene(pDesc);
	}
}

