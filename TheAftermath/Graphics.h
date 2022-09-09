#pragma once
#include "AObject.h"

#include <Windows.h>
#include <d3d12.h>
#include <cstdint>
namespace TheAftermath {

	struct DeviceDesc {
		HWND mHwnd = NULL;
		uint32_t mWidth = 800;
		uint32_t mHeight = 600;
	};

	struct Device : public AObject {
		virtual ID3D12Device7* GetDevice() const = 0;
	};

	Device* CreateDevice(DeviceDesc* pDesc);
}
