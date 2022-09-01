#include "Scene.h"
#include <exception>
namespace TheAftermath {

	struct AScene : public Scene {
		AScene(SceneDesc* pDesc) {
			pDevice = pDesc->pDevice;
			if (!pDevice) {
				throw std::exception("Device is nullptr.");
			}
			// DescriptorHeapPool
			DescriptorHeapPoolDesc poolDesc;
			poolDesc.pDevice = pDevice;
			pPool = CreateDescriptorHeapPool(&poolDesc);
			// gbuffer
			GBufferDesc gbufferDesc;
			gbufferDesc.pDevice = pDevice;
			gbufferDesc.mWidth = pDesc->mWidth;
			gbufferDesc.mHeight = pDesc->mHeight;
			pGBuffer = CreateGBuffer(&gbufferDesc);

		}

		~AScene() {
			RemoveObject(pGBuffer);
			RemoveObject(pPool);
		}

		void Release() { delete this; }

		void Update() {
			pDevice->BeginDraw();
			pDevice->EndDraw();
			pDevice->Present();
		}

		Device* pDevice;
		DescriptorHeapPool* pPool;
		GBuffer* pGBuffer;
	};

	Scene* CreateScene(SceneDesc* pDesc) {
		return new AScene(pDesc);
	}
}

