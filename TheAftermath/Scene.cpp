#pragma once
#include "Scene.h"
#include "Vertex.h"
#include "CommandBuffer.h"
#include "Shader.h"

#include "json.hpp"
#include "d3dx12.h"

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

		auto GBufferVS = LoadCSO(L"GBufferVS.cso");
		auto GBufferPS = LoadCSO(L"GBufferPS.cso");
		pDevice->CreateRootSignature(0, GBufferVS.data(), GBufferVS.size(), IID_PPV_ARGS(&pso.pRootSignature));

		D3D12_INPUT_ELEMENT_DESC gubfferInputDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "ID", 0, DXGI_FORMAT_R32_UINT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
		psoDesc.InputLayout = { gubfferInputDesc, 4 };
		psoDesc.pRootSignature = pso.pRootSignature;
		psoDesc.VS = { GBufferVS.data(), GBufferVS.size() };
		psoDesc.PS = { GBufferPS.data(), GBufferPS.size() };
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
	ID3D12DescriptorHeap* getGBufferDescriptorHeap(ID3D12Device* pDevice) {
		D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
		HeapDesc.NumDescriptors = 10000;
		HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ID3D12DescriptorHeap* pHeap;
		pDevice->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&pHeap));
		return pHeap;
	}

	struct AScene : public Scene {
		AScene(SceneDesc* pDecs) {
			pDevice = pDecs->pDevice;

			CommandBufferDesc commandBufferDesc;
			commandBufferDesc.pDevice = pDevice;
			pCommandBuffer = CreateCommandBuffer(&commandBufferDesc);
			mGbufferPipeline = getGBufferPipeline(pDevice->GetDevice());
			pGbufferDescriptorHeap = getGBufferDescriptorHeap(pDevice->GetDevice());
		}
		~AScene() {
			pGbufferDescriptorHeap->Release();
			mGbufferPipeline.pPSO->Release();
			mGbufferPipeline.pRootSignature->Release();
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

		ID3D12DescriptorHeap* pGbufferDescriptorHeap;
	};

	Scene* CreateScene(SceneDesc* pDecs) {
		return new AScene(pDecs);
	}
}
