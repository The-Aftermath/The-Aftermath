#pragma once
#include "AObject.h"

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
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

	struct DynamicVertexBufferDesc {
		Device* pDevice = nullptr;
	};

	struct DynamicVertexBuffer {

	};
}