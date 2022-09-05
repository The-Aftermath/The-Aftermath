#pragma once
#include "AObject.h"
#include "Vertex.h"

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
#include <string>
#include <vector>
namespace TheAftermath {

	struct DeviceDesc {
		HWND mHwnd = NULL;
		uint32_t mWidth = 800;
		uint32_t mHeight = 600;
	};

	struct Device : public AObject {
		virtual ID3D12Device4* GetDevice() const = 0;
		virtual ID3D12CommandQueue* GetCmdQueue() const = 0;
		virtual void BeginDraw() = 0;
		virtual void EndDraw() = 0;
		virtual void Present() = 0;
		virtual void DrawTexture(ID3D12DescriptorHeap* pSrv, uint32_t index) = 0;
		virtual uint32_t GetFrameCount() const = 0;
		virtual uint32_t GetFrameIndex() const = 0;
	};

	Device* CreateDevice(DeviceDesc* pDesc);

	struct DescriptorHeapPoolDesc {
		Device* pDevice = nullptr;
	};

	struct DescriptorHeapPool : public AObject {
		virtual ID3D12DescriptorHeap* GetCBV_SRV_UAVDescriptorHeap() const = 0;
		virtual ID3D12DescriptorHeap* GetSamplerDescriptorHeap() const = 0;
		virtual D3D12_CPU_DESCRIPTOR_HANDLE AllocCBV_SRV_UAVDescriptor() = 0;
		virtual D3D12_CPU_DESCRIPTOR_HANDLE AllocSamplerDescriptor() = 0;
		virtual void FreeCBV_SRV_UAVDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE handle) = 0;
		virtual void FreeSamplerDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE handle) = 0;
		virtual uint32_t GetCBV_SRV_UAVIndex(D3D12_CPU_DESCRIPTOR_HANDLE handle) const = 0;
		virtual uint32_t GetSamplerIndex(D3D12_CPU_DESCRIPTOR_HANDLE handle) const = 0;
	};

	DescriptorHeapPool* CreateDescriptorHeapPool(DescriptorHeapPoolDesc* pDesc);

	struct GBufferDesc {
		Device* pDevice = nullptr;
		DescriptorHeapPool* pPool = nullptr;
		uint32_t mWidth = 800;
		uint32_t mHeight = 600;
	};

	struct GBuffer : public AObject {
		virtual ID3D12Resource* GetBaseColorResource() const = 0;
		virtual DXGI_FORMAT GetBaseColorFormat() const = 0;
		virtual D3D12_CPU_DESCRIPTOR_HANDLE GetBaseColorRTV() const = 0;
		virtual uint32_t GetBufferWidth() const = 0;
		virtual uint32_t GetBufferHeight() const = 0;
	};

	GBuffer* CreateGBuffer(GBufferDesc* pDesc);

	struct GraphicsPipelineStateDesc {
		Device* pDevice = nullptr;
		std::wstring mVertexShaderCSO;
		std::wstring mPixelShaderCSO;
		D3D12_INPUT_LAYOUT_DESC mInputLayout{ nullptr, 0 };
		uint32_t mNumRenderTargets = 0;
		DXGI_FORMAT mRTVFormats[8]{};
	};

	struct GraphicsPipelineState : public AObject {
		virtual ID3D12PipelineState* GetPipelineState() const = 0;
		virtual ID3D12RootSignature* GetRootSignature() const = 0;
	};

	GraphicsPipelineState* CreateGraphicsPipelineState(GraphicsPipelineStateDesc* pDesc);

	struct DynamicVertexBufferDesc {
		Device* pDevice = nullptr;
	};

	struct DynamicVertexBuffer : public AObject {
		virtual ID3D12Resource* GetVertexBuffer() const = 0;
		virtual uint32_t GetVertexCount() const = 0;
		virtual void Copy(const std::vector<Vertex>& verteics) = 0;
		virtual void AddVertex(const std::vector<Vertex>& verteics) = 0;
	};

	DynamicVertexBuffer* CreateDynamicVertexBuffer(DynamicVertexBufferDesc* pDesc);
}