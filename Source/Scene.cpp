#include "Scene.h"

#include "d3dx12.h"

#include <exception>
#include <vector>
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
			gbufferDesc.pPool = pPool;
			pGBuffer = CreateGBuffer(&gbufferDesc);
			// pipeline
			D3D12_INPUT_ELEMENT_DESC gubfferInputDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "ID", 0, DXGI_FORMAT_R32_UINT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};
			GraphicsPipelineStateDesc pipelineDesc;
			pipelineDesc.mInputLayout = D3D12_INPUT_LAYOUT_DESC{ gubfferInputDesc ,4 };
			pipelineDesc.mVertexShaderCSO = L"GBufferVS.cso";
			pipelineDesc.mPixelShaderCSO = L"GBufferPS.cso";
			pipelineDesc.mNumRenderTargets = 1;
			pipelineDesc.mRTVFormats[0] = pGBuffer->GetBaseColorFormat();
			pipelineDesc.pDevice = pDevice;
			pGBufferPipeline = CreateGraphicsPipelineState(&pipelineDesc);
			// cmdlist and cmdallocator
			auto frameCount = pDevice->GetFrameCount();
			for (uint32_t n = 0; n < frameCount; ++n)
			{
				ID3D12CommandAllocator* pAllocator;
				pDevice->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pAllocator));
				mFrameAllocator.push_back(pAllocator);
			}
			pDevice->GetDevice()->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pList));
		}

		~AScene() {
			for (auto allocator : mFrameAllocator) {
				allocator->Release();
			}
			pList->Release();
			RemoveObject(pGBufferPipeline);
			RemoveObject(pGBuffer);
			RemoveObject(pPool);
		}

		void Release() { delete this; }

		void Update() {
			auto index = pDevice->GetFrameIndex();
			mFrameAllocator[index]->Reset();
			pList->Reset(mFrameAllocator[index], pGBufferPipeline->GetPipelineState());

			//D3D12_VIEWPORT viewport{ 0.F, 0.F, (FLOAT)mWidth, (FLOAT)mHeight, 0.f, 1.f };
			//pList->RSSetViewports(1, &viewport);
			//D3D12_RECT scissorRect{ 0, 0, mWidth, mHeight };
			//pList->RSSetScissorRects(1, &scissorRect);
			//pList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			pList->Close();
			ID3D12CommandList* pLists[] = { pList };
			pDevice->GetCmdQueue()->ExecuteCommandLists(1, pLists);

			pDevice->BeginDraw();
			pDevice->EndDraw();
			pDevice->Present();
		}

		Device* pDevice;
		DescriptorHeapPool* pPool;
		GBuffer* pGBuffer;
		GraphicsPipelineState* pGBufferPipeline;
		//native com
		// cmdlist and cmdallocator
		std::vector<ID3D12CommandAllocator*> mFrameAllocator;
		ID3D12GraphicsCommandList6* pList;
	};

	Scene* CreateScene(SceneDesc* pDesc) {
		return new AScene(pDesc);
	}
}

