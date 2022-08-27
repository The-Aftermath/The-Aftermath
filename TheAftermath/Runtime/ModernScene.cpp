#include "Scene.h"
#include "Camera.h"
#include "../Utility/Utility.h"

#include <combaseapi.h>
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
			//GBuffer Pass
			auto GBufferVS = ReadData(L"GBufferVS.cso");
			auto GBufferPS = ReadData(L"GBufferPS.cso");
			pDevice->GetDevice()->CreateRootSignature(0, GBufferVS.data(), GBufferVS.size(), IID_PPV_ARGS(&pSceneRoot));
			// Camera
			float w = pDevice->GetViewportWidth(), h = pDevice->GetViewportHeight();
			mCamera.SetPerspectiveMatrix(DirectX::XM_PIDIV4, w / h, 1.f, 1000.f);
		}
		~AModernScene() {
			pSceneRoot->Release();
			RemoveTexture(pTexture);
			RemoveGBuffer(pGbuffer);
		}

		void Update() {

			mCamera.Pitch( -0.1f);

			mCamera.UpdateViewMatrix();

			pDevice->BeginDraw();
			pDevice->DrawTexture(pTexture->GetSRV());
			pDevice->EndDraw();
			pDevice->Present();
		}

		GraphicsDevice* pDevice;
		GBuffer* pGbuffer;
		Texture* pTexture;
		//
		Camera mCamera;
		//
		ID3D12RootSignature* pSceneRoot;
	};

	Scene* CreateScene(SceneDesc* pDesc) {
       return new AModernScene(pDesc);
    }
    void RemoveScene(Scene* pScene) {
		auto temp_ptr = dynamic_cast<AModernScene*>(pScene);
		delete temp_ptr;
    }
}