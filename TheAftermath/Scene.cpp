#include "Scene.h"

#include <dxgi1_6.h>

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

		ID3D12CommandQueue* pGraphicsQueue;
		ID3D12CommandQueue* pCopyQueue;
		ID3D12CommandQueue* pComputeQueue;
		IDXGISwapChain4* pSwapChain;

		UINT64 mFenceValue;
		ID3D12Fence* pFence;
		HANDLE mFenceEvent;

	};

	Scene* CreateScene(SceneDesc* pDesc) {
		return new AScene(pDesc);
	}
}