#pragma once
#include "AObject.h"

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
#include <string>
namespace TheAftermath {

	struct DeviceDesc {
		HWND mHwnd = NULL;
		uint32_t mWidth = 800;
		uint32_t mHeight = 600;
	};

	struct Device : public AObject {
		virtual ID3D12Device* GetDevice() const = 0;
		virtual void BeginDraw() = 0;
		virtual void EndDraw() = 0;
		virtual void Present() = 0;
	};

	Device* CreateDevice(DeviceDesc* pDesc);

	struct DescriptorHeapPoolDesc {
		Device* pDevice = nullptr;
	};

	struct DescriptorHeapPool : public AObject {
		virtual ID3D12DescriptorHeap* GetCBV_SRV_UAVDescriptorHeap() const = 0;
		virtual ID3D12DescriptorHeap* GetSamplerDescriptorHeap() const = 0;
	};
	DescriptorHeapPool* CreateDescriptorHeapPool(DescriptorHeapPoolDesc* pDesc);

	struct GBufferDesc {
		Device* pDevice = nullptr;
		uint32_t mWidth = 800;
		uint32_t mHeight = 600;
	};
	struct GBuffer : public AObject {

	};


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

	};
}