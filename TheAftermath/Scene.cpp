#include "Scene.h"
#include "bgfx/c99/bgfx.h"
namespace TheAftermath {
	struct AScene : public Scene {
		AScene(SceneDesc* pDesc) {
			pDevice = pDesc->pDevice;

			mWidth = pDesc->mWidth;
			mHeight = pDesc->mHeight;

			//bgfx_set_debug(BGFX_DEBUG_TEXT);
			// init rendering
			//mBaseColorHandle = bgfx_create_frame_buffer((uint16_t)mWidth, (uint16_t)mHeight, BGFX_TEXTURE_FORMAT_RGBA32F, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
		}

		~AScene() {
			bgfx_destroy_frame_buffer(mBaseColorHandle);
		}

		void Release() {
			delete this;
		}

		void Update() {
		}

		Device* pDevice;

		uint32_t mWidth;
		uint32_t mHeight;

		bgfx_frame_buffer_handle_t mBaseColorHandle;
	};

	Scene* CreateScene(SceneDesc* pDesc) {
		return new AScene(pDesc);
	}
}