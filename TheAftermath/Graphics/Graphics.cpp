#include "Graphics.h"

#include <dxgi1_6.h>
#include <combaseapi.h>

extern "C"
{
    __declspec(dllexport) extern const UINT D3D12SDKVersion = D3D12_SDK_VERSION;
    __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\";
}

namespace TheAftermath {

    class AGraphicsDevice : public GraphicsDevice {
    public:
        AGraphicsDevice(GraphicsDeviceDesc* pDesc) {
            ID3D12Debug* debugController = nullptr;
#if defined(_DEBUG)
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
            {
                debugController->EnableDebugLayer();
            }
            CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&pFactory));
#else
            CreateDXGIFactory2(0, IID_PPV_ARGS(&pFactory));
#endif
            D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&pDevice));

            D3D12_COMMAND_QUEUE_DESC queueDesc{};
            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pMainQueue));
            
            DXGI_SWAP_CHAIN_DESC1 scDesc{};
            scDesc.BufferCount = 3;
            scDesc.Width = pDesc->mWidth;
            scDesc.Height = pDesc->mHeight;
            scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            scDesc.SampleDesc.Count = 1;
            scDesc.SampleDesc.Quality = 0;
            scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
            scDesc.Scaling = DXGI_SCALING_STRETCH;

            DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc{};
            fsSwapChainDesc.Windowed = TRUE;
            pFactory->CreateSwapChainForHwnd(pMainQueue, pDesc->mHwnd, &scDesc, &fsSwapChainDesc, nullptr, &pSwapChain);
            pFactory->MakeWindowAssociation(pDesc->mHwnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

            if (debugController) {
                debugController->Release();
            }
        }

        ~AGraphicsDevice() {
            pMainQueue->Release();
            pDevice->Release();
            pFactory->Release();
            pSwapChain->Release();
        }

        ID3D12Device* GetDevice() const {
            return pDevice;
        }

        void Present() {
            pSwapChain->Present(1, 0);
        }

        ID3D12Device11* pDevice = nullptr;
        IDXGIFactory7* pFactory = nullptr;
        ID3D12CommandQueue* pMainQueue = nullptr;
        IDXGISwapChain1* pSwapChain = nullptr;
    };

    GraphicsDevice* CreateGraphicsDevice(GraphicsDeviceDesc* pDesc) {
        return new AGraphicsDevice(pDesc);
    }

    void RemoveGraphicsDevice(GraphicsDevice* pDevice) {
        auto temp_ptr = dynamic_cast<AGraphicsDevice*>(pDevice);
        delete temp_ptr;
    }

}
