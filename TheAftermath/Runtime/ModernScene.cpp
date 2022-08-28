#include "Scene.h"
#include "Camera.h"
#include "../Utility/Utility.h"

#include "d3dx12.h"

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
			pDevice->GetDevice()->CreateRootSignature(0, GBufferVS.data(), GBufferVS.size(), IID_PPV_ARGS(&pGBufferRoot));
			
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
			psoDesc.InputLayout = { nullptr, 0 };
			psoDesc.pRootSignature = pGBufferRoot;
			psoDesc.VS = { GBufferVS.data(), GBufferVS.size() };
			psoDesc.PS = { GBufferPS.data(), GBufferPS.size() };
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.SampleMask = 0xffffffff;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;
			pDevice->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pGBufferPipeline));

			
			// Camera
			float w = pDevice->GetViewportWidth(), h = pDevice->GetViewportHeight();
			mCamera.SetPerspectiveMatrix(DirectX::XM_PIDIV4, w / h, 1.f, 1000.f);
		}
		~AModernScene() {
			pGBufferPipeline->Release();
			pGBufferRoot->Release();
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
		ID3D12RootSignature* pGBufferRoot;
		ID3D12PipelineState* pGBufferPipeline;
	};

	Scene* CreateScene(SceneDesc* pDesc) {
       return new AModernScene(pDesc);
    }
    void RemoveScene(Scene* pScene) {
		auto temp_ptr = dynamic_cast<AModernScene*>(pScene);
		delete temp_ptr;
    }
}