#include "Graphics.h"
#include "../Utility/Utility.h"

#include "WICTextureLoader.h"
#include "ResourceUploadBatch.h"

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
            auto OutputVS = ReadData(L"OutputVS.cso");
            auto OutputPS = ReadData(L"OutputPS.cso");
            pDevice->CreateRootSignature(0, OutputVS.data(), OutputVS.size(), IID_PPV_ARGS(&pOutputRoot));

            D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
            psoDesc.InputLayout = { nullptr, 0 };
            psoDesc.pRootSignature = pOutputRoot;
            psoDesc.VS = { OutputVS.data(), OutputVS.size() };
            psoDesc.PS = { OutputPS.data(), OutputPS.size() };
            psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            psoDesc.SampleMask = 0xffffffff;
            psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            psoDesc.NumRenderTargets = 1;
            psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
            psoDesc.SampleDesc.Count = 1;
            pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pOutputPipeline));
            // swapchain rtv
            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
            rtvHeapDesc.NumDescriptors = 3;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&p_SC_RTVHeap));
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(p_SC_RTVHeap->GetCPUDescriptorHandleForHeapStart());
            mRTVDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            for (uint32_t n = 0; n < 3; ++n)
            {
                ID3D12Resource* _res;
                pSwapChain->GetBuffer(n, IID_PPV_ARGS(&_res));
                pDevice->CreateRenderTargetView(_res, nullptr, rtvHandle);
                rtvHandle.Offset(1, mRTVDescriptorSize);
                _res->Release();
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
            p_SC_RTVHeap->Release();

            pOutputPipeline->Release();
            pOutputRoot->Release();

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
            pFrameAllocator[index]->Reset();
            pList->Reset(pFrameAllocator[index], pOutputPipeline);
         
            CD3DX12_VIEWPORT viewport(0.F, 0.F, (FLOAT)mWidth, (FLOAT)mHeight);
            pList->RSSetViewports(1, &viewport);
            CD3DX12_RECT scissorRect(0, 0, mWidth, mHeight);
            pList->RSSetScissorRects(1, &scissorRect);
            pList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            ID3D12Resource* renderTarget;
            pSwapChain->GetBuffer(index, IID_PPV_ARGS(&renderTarget));
            auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
            pList->ResourceBarrier(1, &barrier);

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(p_SC_RTVHeap->GetCPUDescriptorHandleForHeapStart(), index, mRTVDescriptorSize);
            pList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

            renderTarget->Release();
        }
        virtual void EndDraw() {
            auto index = GetFrameIndex();
            ID3D12Resource* renderTarget;
            pSwapChain->GetBuffer(index, IID_PPV_ARGS(&renderTarget));
            auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            pList->ResourceBarrier(1, &barrier);
            pList->Close();
            ID3D12CommandList* pLists[] = { pList };
            pMainQueue->ExecuteCommandLists(1, pLists);

            renderTarget->Release();
        }

        void DrawTexture(ID3D12DescriptorHeap* pSrv) {
            ID3D12DescriptorHeap* ppHeaps[] = { pSrv };
            pList->SetDescriptorHeaps(1, ppHeaps);
            pList->SetGraphicsRootSignature(pOutputRoot);

            pList->DrawInstanced(3, 1, 0, 0);
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
        ID3D12RootSignature* pOutputRoot;

        ID3D12DescriptorHeap* p_SC_RTVHeap;
        UINT mRTVDescriptorSize;

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

    class ATexture : public Texture {
    public:
        ATexture(TextureDesc* pDesc) {
            pDevice = pDesc->pDevice;

            DirectX::ResourceUploadBatch resourceUpload(pDevice->GetDevice());
            resourceUpload.Begin();

            DirectX::CreateWICTextureFromFile(pDevice->GetDevice(), resourceUpload, pDesc->mFilePath.c_str(),
                &pTex, false);

            auto uploadResourcesFinished = resourceUpload.End(pDevice->GetImmediateCommandQueue());
            uploadResourcesFinished.wait();

            D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
            HeapDesc.NumDescriptors = 1;
            HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            pDevice->GetDevice()->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&pSrv));
            pDevice->GetDevice()->CreateShaderResourceView(pTex, nullptr, pSrv->GetCPUDescriptorHandleForHeapStart());
        }
        ~ATexture() {
            pTex->Release();
            pSrv->Release();
        }

        ID3D12DescriptorHeap* GetSRV() const {
            return pSrv;
        }

        GraphicsDevice* pDevice;
        ID3D12Resource* pTex;
        ID3D12DescriptorHeap* pSrv;
    };

    Texture* CreateTexture(TextureDesc* pDesc) {
        return new ATexture(pDesc);
    }
    void RemoveTexture(Texture* pTexture) {
        auto temp_ptr = dynamic_cast<ATexture*>(pTexture);
        delete temp_ptr;
    }

    class AGBuffer : public GBuffer {
    public:
        AGBuffer(GBufferDesc* pDesc) {
            pDevice = pDesc->pDevice;

            auto baseResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, pDesc->mWidth, pDesc->mHeight);
            baseResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            auto baseHeapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            pDevice->GetDevice()->CreateCommittedResource(
                &baseHeapDesc,
                D3D12_HEAP_FLAG_NONE,
                &baseResDesc,
                D3D12_RESOURCE_STATE_COMMON,
                nullptr,
                IID_PPV_ARGS(&pBaseColor));

            D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
            HeapDesc.NumDescriptors = 4;
            HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            pDevice->GetDevice()->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&pGBufferRTVDescriptorHeap));
            // base color rtv
            pDevice->GetDevice()->CreateRenderTargetView(pBaseColor, nullptr, pGBufferRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
        }
        ~AGBuffer() {
            pGBufferRTVDescriptorHeap->Release();
            pBaseColor->Release();
        }

        ID3D12Resource* GetBaseColorResource() const {
            return pBaseColor;
        }

        DXGI_FORMAT GetBaseColorFormat() const {
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE GetBaseColorRTV() const {
            return pGBufferRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        }

        GraphicsDevice* pDevice;
        ID3D12Resource* pBaseColor;
        ID3D12DescriptorHeap* pGBufferRTVDescriptorHeap;
    };

    GBuffer* CreateGBuffer(GBufferDesc* pDesc) {
        return new AGBuffer(pDesc);
    }
    void RemoveGBuffer(GBuffer* pBuffer) {
        auto temp_ptr = dynamic_cast<AGBuffer*>(pBuffer);
        delete temp_ptr;
    }

}
