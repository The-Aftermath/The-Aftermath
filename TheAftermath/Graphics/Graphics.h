#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cstdint>
#include <string>

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
        virtual ID3D12CommandQueue* GetImmediateCommandQueue() const = 0;
        virtual ID3D12CommandAllocator* GetActiveAllocator() const = 0;
        virtual ID3D12GraphicsCommandList* GetActiveList() const = 0;
        virtual uint32_t GetFrameIndex() const = 0;
        virtual void Present() = 0;
        virtual void Wait() = 0;
        virtual uint32_t GetViewportWidth() const = 0;
        virtual uint32_t GetViewportHeight() const = 0;
        virtual void BeginDraw() = 0;
        virtual void EndDraw() = 0;
        virtual void DrawTexture(ID3D12DescriptorHeap* pSrv) = 0;
        virtual void DrawTexture(ID3D12DescriptorHeap* pSrv, uint32_t index) = 0;
    };

    GraphicsDevice* CreateGraphicsDevice(GraphicsDeviceDesc* pDesc);
    void RemoveGraphicsDevice(GraphicsDevice* pDevice);

    struct TextureDesc {
        GraphicsDevice* pDevice;
        std::wstring mFilePath;
    };
    class Texture {
    public:
        virtual ID3D12DescriptorHeap* GetSRV() const = 0;
    };

    Texture* CreateTexture(TextureDesc* pDesc);
    void RemoveTexture(Texture* pTexture);

    struct GBufferDesc {
        GraphicsDevice* pDevice;
        uint32_t mWidth;
        uint32_t mHeight;
    };
    class GBuffer {
    public:
        virtual ID3D12Resource* GetBaseColorResource() const = 0;
        virtual DXGI_FORMAT GetBaseColorFormat() const = 0;
        virtual D3D12_CPU_DESCRIPTOR_HANDLE GetBaseColorRTV() const = 0;
    };

    GBuffer* CreateGBuffer(GBufferDesc* pDesc);
    void RemoveGBuffer(GBuffer* pBuffer);
}
