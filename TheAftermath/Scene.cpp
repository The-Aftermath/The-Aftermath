#include "Scene.h"

namespace TheAftermath {
	struct AScene : public Scene {
		AScene(SceneDesc* pDesc) {
			pDevice = pDesc->pDevice;

			mWidth = pDesc->mWidth;
			mHeight = pDesc->mHeight;

		}

		~AScene() {
			//bgfx_destroy_frame_buffer(mGBufferFBOHandle);
		}

		void Release() {
			delete this;
		}

		void Update() {

		}

		Device* pDevice;

		uint32_t mWidth;
		uint32_t mHeight;

	};

	Scene* CreateScene(SceneDesc* pDesc) {
		return new AScene(pDesc);
	}
}