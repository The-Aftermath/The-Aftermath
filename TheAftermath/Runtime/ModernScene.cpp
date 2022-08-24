#include "Scene.h"
namespace TheAftermath {
	class AModernScene : public Scene {
	public:
		AModernScene(SceneDesc* pDesc) {
			pDevice = pDesc->pDevice;
			//Gbuffer
			GBufferDesc gbufferDesc;
			gbufferDesc.pDevice = pDevice;
			gbufferDesc.mWidth = pDevice->GetViewportWidth();
			gbufferDesc.mHeight = pDevice->GetViewportHeight();
			pGbuffer = CreateGBuffer(&gbufferDesc);
			//
			TextureDesc texDesc;
			texDesc.pDevice = pDevice;
			texDesc.mFilePath = L"Asset/test.png";
			pTexture = CreateTexture(&texDesc);
		}
		~AModernScene() {
			RemoveTexture(pTexture);
			RemoveGBuffer(pGbuffer);
		}

		void Update() {




			pDevice->BeginDraw();
			pDevice->DrawTexture(pTexture->GetSRV());
			pDevice->EndDraw();
			pDevice->Present();
		}

		GraphicsDevice* pDevice;
		GBuffer* pGbuffer;
		Texture* pTexture;
	};

	Scene* CreateScene(SceneDesc* pDesc) {
       return new AModernScene(pDesc);
    }
    void RemoveScene(Scene* pScene) {
		auto temp_ptr = dynamic_cast<AModernScene*>(pScene);
		delete temp_ptr;
    }
}