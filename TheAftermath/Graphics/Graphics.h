#pragma once
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
namespace TheAftermath {
    struct GraphicsDeviceDesc {
        HWND mHwnd;
        uint32_t mWidth;
        uint32_t mHeight;
    };

    class GraphicsDevice {
    public:
        virtual ID3D12Device* GetDevice() const = 0;
        virtual IDXGISwapChain* GetSwapChain() const = 0;
        virtual void Present() = 0;
    };

    GraphicsDevice* CreateGraphicsDevice(GraphicsDeviceDesc* pDesc);
    void RemoveGraphicsDevice(GraphicsDevice* pDevice);
}
