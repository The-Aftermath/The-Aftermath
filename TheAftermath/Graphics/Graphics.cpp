#include "Graphics.h"

#include "RenderTargetState.h"

#include "d3dx12.h"

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
            mWidth = pDesc->mWidth;
            mHeight = pDesc->mHeight;

            _CreateCmd();
            // pipeline
            D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
            psoDesc.InputLayout = { 0, 0 };
            //psoDesc.pRootSignature = pSceneRoot;
            //psoDesc.VS = { SceneVSBlob.data(), SceneVSBlob.size() };
            //psoDesc.PS = { ScenePSBlob.data(), ScenePSBlob.size() };
            //psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            //psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            //psoDesc.SampleMask = 0xffffffff;
            //psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            //psoDesc.NumRenderTargets = 1;
            //psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
            //psoDesc.SampleDesc.Count = 1;
            //pDevice->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pScenePSO));
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

            pFrameAllocator[0]->Release();
            pFrameAllocator[1]->Release();
            pFrameAllocator[2]->Release();
            pList->Release();

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

        virtual uint32_t GetViewportWidth() const {
            return mWidth;
        }
        virtual uint32_t GetViewportHeight() const {
            return mHeight;
        }

        virtual void BeginDraw() {
            auto index = GetFrameIndex();
        }
        virtual void EndDraw() {

        }

        void _CreateCmd() {
            for (uint32_t n = 0; n < 3; ++n)
            {
                pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pFrameAllocator[n]));
            }
            pDevice->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pList));
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

        ID3D12CommandAllocator* pFrameAllocator[3];
        ID3D12GraphicsCommandList8* pList;

        ID3D12PipelineState* pOutputPipeline;

        uint32_t mWidth;
        uint32_t mHeight;
    };

    GraphicsDevice* CreateGraphicsDevice(GraphicsDeviceDesc* pDesc) {
        return new AGraphicsDevice(pDesc);
    }

    void RemoveGraphicsDevice(GraphicsDevice* pDevice) {
        auto temp_ptr = dynamic_cast<AGraphicsDevice*>(pDevice);
        delete temp_ptr;
    }


    class AGBuffer : public GBuffer {
    public:
        AGBuffer(GBufferDesc* pDesc) {
            pDevice = pDesc->pDevice;

            auto baseResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, pDesc->mWidth, pDesc->mHeight);
            auto baseHeapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            pDevice->GetDevice()->CreateCommittedResource(
                &baseHeapDesc,
                D3D12_HEAP_FLAG_NONE,
                &baseResDesc,
                D3D12_RESOURCE_STATE_COMMON,
                nullptr,
                IID_PPV_ARGS(&pBaseColor));
        }
        ~AGBuffer() {
            pBaseColor->Release();
        }

        ID3D12Resource* GetBaseColorResource() const {
            return pBaseColor;
        }

        GraphicsDevice* pDevice;
        ID3D12Resource* pBaseColor;
    };

    GBuffer* CreateGBuffer(GBufferDesc* pDesc) {
        return new AGBuffer(pDesc);
    }
    void RemoveGBuffer(GBuffer* pBuffer) {
        auto temp_ptr = dynamic_cast<AGBuffer*>(pBuffer);
        delete temp_ptr;
    }

}
