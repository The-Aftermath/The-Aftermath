#pragma once
#include "Scene.h"
#include "Vertex.h"
#include "CommandBuffer.h"

#include "json.hpp"
#include "d3dx12.h"
#include "dxcapi.h"

#include <dxgi1_6.h>
#include <vector>

namespace TheAftermath {

	struct pipeline
	{
		ID3D12PipelineState* pPSO = nullptr;
		ID3D12RootSignature* pRootSignature = nullptr;
	};
	
	pipeline getGBufferPipeline(ID3D12Device *pDevice) {
		pipeline pso;
		D3D12_INPUT_ELEMENT_DESC gubfferInputDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "ID", 0, DXGI_FORMAT_R32_UINT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
		psoDesc.InputLayout = { gubfferInputDesc, 4 };
		//psoDesc.pRootSignature = pGBufferRoot;
		//psoDesc.VS = { GBufferVS.data(), GBufferVS.size() };
		//psoDesc.PS = { GBufferPS.data(), GBufferPS.size() };
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = 0xffffffff;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		psoDesc.SampleDesc.Count = 1;
		pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso.pPSO));

		return pso;
	}

	struct AScene : public Scene {
		AScene(SceneDesc* pDecs) {
			pDevice = pDecs->pDevice;

			CommandBufferDesc commandBufferDesc;
			commandBufferDesc.pDevice = pDevice;
			pCommandBuffer = CreateCommandBuffer(&commandBufferDesc);
		}
		~AScene() {
			RemoveObject(pCommandBuffer);
		}
		void Release() { delete this; }

		void Update() {
			pCommandBuffer->Reset();
			pCommandBuffer->Close();
			pDevice->Execute(pCommandBuffer->GetCmdList());
			pDevice->Present();
		}
		void LoadModel(const wchar_t* modelFilePath) {

		}

		Device* pDevice;

		CommandBuffer* pCommandBuffer;

		std::vector<Vertex> mAllVertex;
		std::vector<uint32_t> mAllIndex;

		std::vector<Vertex> mVisibleVertex;
		std::vector<uint32_t> mVisibleIndex;

		pipeline mGbufferPipeline;
	};

	Scene* CreateScene(SceneDesc* pDecs) {
		return new AScene(pDecs);
	}
}
