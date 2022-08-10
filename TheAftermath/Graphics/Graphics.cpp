#include "Graphics.h"

#include <combaseapi.h>
#include <tuple>

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
            pSwapChain->QueryInterface(&m_swapChain);
            pFactory->MakeWindowAssociation(pDesc->mHwnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

            pDevice->CreateFence(m_fenceValues[m_backBufferIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
            m_fenceValues[m_backBufferIndex]++; 
            m_Handle = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);

            if (debugController) {
                debugController->Release();
            }
        }

        ~AGraphicsDevice() {
            const UINT64 fenceValue = m_fenceValues[m_backBufferIndex];
            if (SUCCEEDED(pMainQueue->Signal(pFence, fenceValue)))
            {
                if (SUCCEEDED(pFence->SetEventOnCompletion(fenceValue, m_Handle)))
                {
                    std::ignore = WaitForSingleObjectEx(m_Handle, INFINITE, FALSE);
                    m_fenceValues[m_backBufferIndex]++;
                }
            }

            pMainQueue->Release();
            pDevice->Release();
            pFactory->Release();
            pSwapChain->Release();
            m_swapChain->Release();
            pFence->Release();
            CloseHandle(m_Handle);
        }

        ID3D12Device* GetDevice() const {
            return pDevice;
        }
        IDXGISwapChain* GetSwapChain() const {
            return pSwapChain;
        }

        ID3D12CommandQueue* GetImmediateCommandQueue() const {
            return pMainQueue;
        }
        void Wait() {
            const UINT64 fenceValue = m_fenceValues[m_backBufferIndex];
            if (SUCCEEDED(pMainQueue->Signal(pFence, fenceValue)))
            {
                if (SUCCEEDED(pFence->SetEventOnCompletion(fenceValue, m_Handle)))
                {
                    std::ignore = WaitForSingleObjectEx(m_Handle, INFINITE, FALSE);
                    m_fenceValues[m_backBufferIndex]++;
                }
            }
        }

        void Present() {
            m_swapChain->Present(1, 0);

            const UINT64 currentFenceValue = m_fenceValues[m_backBufferIndex];
            pMainQueue->Signal(pFence, currentFenceValue);

            m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

            if (pFence->GetCompletedValue() < m_fenceValues[m_backBufferIndex])
            {
                pFence->SetEventOnCompletion(m_fenceValues[m_backBufferIndex], m_Handle);
                std::ignore = WaitForSingleObjectEx(m_Handle, INFINITE, FALSE);
            }

            m_fenceValues[m_backBufferIndex] = currentFenceValue + 1;
        }


        uint32_t GetFrameIndex() const {
            return m_swapChain->GetCurrentBackBufferIndex();
        }

        ID3D12Device11* pDevice = nullptr;
        IDXGIFactory7* pFactory = nullptr;
        ID3D12CommandQueue* pMainQueue = nullptr;
        IDXGISwapChain1* pSwapChain = nullptr;
        IDXGISwapChain4* m_swapChain = nullptr;
        ID3D12Fence1* pFence = nullptr;

        UINT64 m_fenceValues[3]{ 0,0,0 };
        UINT m_backBufferIndex = 0;
        HANDLE m_Handle = nullptr;
    };

    GraphicsDevice* CreateGraphicsDevice(GraphicsDeviceDesc* pDesc) {
        return new AGraphicsDevice(pDesc);
    }

    void RemoveGraphicsDevice(GraphicsDevice* pDevice) {
        auto temp_ptr = dynamic_cast<AGraphicsDevice*>(pDevice);
        delete temp_ptr;
    }

}
