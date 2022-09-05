#include "Scene.h"

#include "d3dx12.h"

#include <exception>
#include <vector>
#include <DirectXMath.h>
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
			// vb
			DynamicVertexBufferDesc vbDesc;
			vbDesc.pDevice = pDevice;
			pVB = CreateDynamicVertexBuffer(&vbDesc);
			// cb
			auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(GBufferCB_cpu));
			pDevice->GetDevice()->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pGBufferCB));
			void* pData;
			pGBufferCB->Map(0, nullptr, &pData);
			pGBufferCB_CPU = (GBufferCB_cpu*)pData;
			// cbv
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
            cbvDesc.BufferLocation = pGBufferCB->GetGPUVirtualAddress();
			cbvDesc.SizeInBytes = sizeof(GBufferCB_cpu);
			pDevice->GetDevice()->CreateConstantBufferView(&cbvDesc, pPool->AllocCBV_SRV_UAVDescriptor());
		}

		~AScene() {
			pGBufferCB->Unmap(0, nullptr);
			pGBufferCB->Release();

			RemoveObject(pVB);

			for (auto allocator : mFrameAllocator) {
				allocator->Release();
			}
			
			pList->Release();
			RemoveObject(pGBufferPipeline);
			RemoveObject(pGBuffer);
			RemoveObject(pPool);
		}

		void Release() { delete this; }

		void UpdateGBuffer() {
			auto index = pDevice->GetFrameIndex();
			mFrameAllocator[index]->Reset();
			pList->Reset(mFrameAllocator[index], pGBufferPipeline->GetPipelineState());

			D3D12_VIEWPORT viewport{ 0.F, 0.F, (FLOAT)pGBuffer->GetBufferWidth(), (FLOAT)pGBuffer->GetBufferHeight(), 0.f, 1.f };
			pList->RSSetViewports(1, &viewport);
			D3D12_RECT scissorRect{ 0, 0, pGBuffer->GetBufferWidth(), pGBuffer->GetBufferHeight() };
			pList->RSSetScissorRects(1, &scissorRect);
			pList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(pGBuffer->GetBaseColorResource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);
			pList->ResourceBarrier(1, &barrier);

			ID3D12DescriptorHeap* pHeaps[] = { pPool->GetCBV_SRV_UAVDescriptorHeap() };
			pList->SetDescriptorHeaps(1, pHeaps);
			pList->SetGraphicsRootSignature(pGBufferPipeline->GetRootSignature());
			//pList->SetGraphicsRoot32BitConstant(1, index, 0);
			//pList->DrawInstanced(3, 1, 0, 0);

			pList->DrawInstanced(pVB->GetVertexCount(), 1, 0, 0);
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[] = { pGBuffer->GetBaseColorRTV() };
			pList->OMSetRenderTargets(1, rtvHandles, FALSE, nullptr);


			barrier = CD3DX12_RESOURCE_BARRIER::Transition(pGBuffer->GetBaseColorResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
			pList->ResourceBarrier(1, &barrier);
			pList->Close();
			ID3D12CommandList* pLists[] = { pList };
			pDevice->GetCmdQueue()->ExecuteCommandLists(1, pLists);
		}

		void Update() {
			//

			//
			UpdateGBuffer();

			pDevice->BeginDraw();
			pDevice->DrawTexture(pPool->GetCBV_SRV_UAVDescriptorHeap(), pPool->GetCBV_SRV_UAVIndex(pGBuffer->GetBaseColorRTV()));
			pDevice->EndDraw();
			pDevice->Present();
		}

		Device* pDevice;
		DescriptorHeapPool* pPool;
		GBuffer* pGBuffer;
		GraphicsPipelineState* pGBufferPipeline;
		DynamicVertexBuffer* pVB;
		//native com
		// cmdlist and cmdallocator
		std::vector<ID3D12CommandAllocator*> mFrameAllocator;
		ID3D12GraphicsCommandList6* pList;
		// gbuffer cb
		struct GBufferCB_cpu {
			float V[16];
			float P[16];
			float Light[4];
			float padding[28];
		};
		static_assert((sizeof(GBufferCB_cpu) % 256) == 0, "Constant Buffer size must be 256-byte aligned");
		ID3D12Resource* pGBufferCB;
		GBufferCB_cpu* pGBufferCB_CPU;
	};

	Scene* CreateScene(SceneDesc* pDesc) {
		return new AScene(pDesc);
	}
}

