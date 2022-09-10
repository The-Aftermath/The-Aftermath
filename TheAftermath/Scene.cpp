#include "Scene.h"
#include "bgfx/c99/bgfx.h"
namespace TheAftermath {
	struct AScene : public Scene {
		AScene(SceneDesc* pDesc) {
			pDevice = pDesc->pDevice;

			mWidth = pDesc->mWidth;
			mHeight = pDesc->mHeight;

			mGBufferBaseColorView = 0;
			mGBufferDepthView = 1;

			const uint64_t tsFlags = 0
				| BGFX_SAMPLER_MIN_POINT
				| BGFX_SAMPLER_MAG_POINT
				| BGFX_SAMPLER_MIP_POINT
				| BGFX_SAMPLER_U_CLAMP
				| BGFX_SAMPLER_V_CLAMP
				;
			bgfx_attachment_t gbufferAt[2];
			mGBufferBaseColorHandle = bgfx_create_texture_2d(uint16_t(mWidth), uint16_t(mHeight), false, 1, BGFX_TEXTURE_FORMAT_RGBA32F, BGFX_TEXTURE_RT | tsFlags, nullptr);
			bgfx_attachment_init(&gbufferAt[0], mGBufferBaseColorHandle, BGFX_ACCESS_WRITE, 0, 1, 0, BGFX_RESOLVE_NONE);

			mGBufferDepthHandle = bgfx_create_texture_2d(uint16_t(mWidth), uint16_t(mHeight), false, 1, BGFX_TEXTURE_FORMAT_D32F, BGFX_TEXTURE_RT | tsFlags, nullptr);
			bgfx_attachment_init(&gbufferAt[1], mGBufferDepthHandle, BGFX_ACCESS_WRITE, 0, 1, 0, BGFX_RESOLVE_NONE);

			mGBufferFBOHandle = bgfx_create_frame_buffer_from_attachment(2, gbufferAt, true);
		
			bgfx_set_view_clear(0
				, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
				, 0x00ff30ff
				, 1.0f
				, 0
			);
		}

		~AScene() {
			//bgfx_destroy_frame_buffer(mGBufferFBOHandle);
		}

		void Release() {
			delete this;
		}

		void Update() {
			bgfx_set_view_rect(0, 0, 0, uint16_t(mWidth), uint16_t(mHeight));
			bgfx_touch(0);

			bgfx_frame(false);
		}

		Device* pDevice;

		uint32_t mWidth;
		uint32_t mHeight;

		bgfx_texture_handle_t mGBufferBaseColorHandle;
		bgfx_texture_handle_t mGBufferDepthHandle;
		bgfx_frame_buffer_handle_t mGBufferFBOHandle;

		bgfx_view_id_t mGBufferBaseColorView;
		bgfx_view_id_t mGBufferDepthView;
	};

	Scene* CreateScene(SceneDesc* pDesc) {
		return new AScene(pDesc);
	}
}