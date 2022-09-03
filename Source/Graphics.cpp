#include "Graphics.h"
#include "Utility.h"

#include "d3dx12.h"

#include <combaseapi.h>
#include <winerror.h>
#include <exception>
#include <wrl.h>
#include <vector>

#define FRAME_COUNT 3
namespace TheAftermath {

	struct ADevice : public Device {
		ADevice(DeviceDesc* pDesc) {

			if (pDesc->mHwnd == NULL)
			{
				throw std::exception("HWND is NULL.");
			}
			//debug 
#if defined(_DEBUG)
			Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();
			}
#endif
			// device
			auto hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&pDevice));
			if (hr != S_OK) {
				throw std::exception("Your adapter does not support DX12.2.");
			}
			// cmd
			D3D12_COMMAND_QUEUE_DESC queueDesc{};
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pMainQueue));
			for (uint32_t n = 0; n < FRAME_COUNT; ++n)
			{
				pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pFrameAllocator[n]));
			}
			pDevice->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pList));
			// swapchain
			Microsoft::WRL::ComPtr<IDXGIFactory4> pFactory;
			CreateDXGIFactory2(0, IID_PPV_ARGS(&pFactory));
			DXGI_SWAP_CHAIN_DESC1 scDesc{};
			scDesc.BufferCount = FRAME_COUNT;
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
			Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
			pFactory->CreateSwapChainForHwnd(pMainQueue, pDesc->mHwnd, &scDesc, &fsSwapChainDesc, nullptr, &swapChain);
			swapChain->QueryInterface(&pSwapChain);
			pFactory->MakeWindowAssociation(pDesc->mHwnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
			// fence
			pDevice->CreateFence(m_fenceValues[m_backBufferIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
			m_fenceValues[m_backBufferIndex]++;
			m_Handle = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
			// sc rtv
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = FRAME_COUNT;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&p_SC_RTVHeap));
			auto rtvHandle = p_SC_RTVHeap->GetCPUDescriptorHandleForHeapStart();
			mRTVDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			for (uint32_t n = 0; n < FRAME_COUNT; ++n)
			{
				Microsoft::WRL::ComPtr<ID3D12Resource> _res;
				pSwapChain->GetBuffer(n, IID_PPV_ARGS(&_res));
				pDevice->CreateRenderTargetView(_res.Get(), nullptr, rtvHandle);
				rtvHandle.ptr += mRTVDescriptorSize;
			}
			//
			mWidth = pDesc->mWidth;
			mHeight = pDesc->mHeight;
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
		}
		~ADevice() {
			const UINT64 fenceValue = m_fenceValues[m_backBufferIndex];
			if (SUCCEEDED(pMainQueue->Signal(pFence, fenceValue)))
			{
				if (SUCCEEDED(pFence->SetEventOnCompletion(fenceValue, m_Handle)))
				{
					WaitForSingleObjectEx(m_Handle, INFINITE, FALSE);
					m_fenceValues[m_backBufferIndex]++;
				}
			}

			pOutputPipeline->Release();
			pOutputRoot->Release();

			p_SC_RTVHeap->Release();

			pFence->Release();

			pFrameAllocator[0]->Release();
			pFrameAllocator[1]->Release();
			pFrameAllocator[2]->Release();
			pList->Release();

			pMainQueue->Release();
			pSwapChain->Release();

			pDevice->Release();

			CloseHandle(m_Handle);
		}

		void Release() { delete this; }

		ID3D12Device4* GetDevice() const {
			return pDevice;
		}
		ID3D12CommandQueue* GetCmdQueue() const {
			return pMainQueue;
		}


		virtual void BeginDraw() {
			auto index = m_backBufferIndex;
			pFrameAllocator[index]->Reset();
			pList->Reset(pFrameAllocator[index], pOutputPipeline);

			D3D12_VIEWPORT viewport{ 0.F, 0.F, (FLOAT)mWidth, (FLOAT)mHeight, 0.f, 1.f };
			pList->RSSetViewports(1, &viewport);
			D3D12_RECT scissorRect{ 0, 0, mWidth, mHeight };
			pList->RSSetScissorRects(1, &scissorRect);
			pList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			Microsoft::WRL::ComPtr<ID3D12Resource> renderTarget;
			pSwapChain->GetBuffer(index, IID_PPV_ARGS(&renderTarget));
			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			pList->ResourceBarrier(1, &barrier);

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(p_SC_RTVHeap->GetCPUDescriptorHandleForHeapStart(), index, mRTVDescriptorSize);
			pList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
		}

		virtual void EndDraw() {
			auto index = m_backBufferIndex;
			Microsoft::WRL::ComPtr<ID3D12Resource> renderTarget;

			pSwapChain->GetBuffer(index, IID_PPV_ARGS(&renderTarget));
			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			pList->ResourceBarrier(1, &barrier);
			pList->Close();
			ID3D12CommandList* pLists[] = { pList };
			pMainQueue->ExecuteCommandLists(1, pLists);
		}

		void Present() {
			pSwapChain->Present(1, 0);

			const UINT64 currentFenceValue = m_fenceValues[m_backBufferIndex];
			pMainQueue->Signal(pFence, currentFenceValue);

			m_backBufferIndex = pSwapChain->GetCurrentBackBufferIndex();

			if (pFence->GetCompletedValue() < m_fenceValues[m_backBufferIndex])
			{
				pFence->SetEventOnCompletion(m_fenceValues[m_backBufferIndex], m_Handle);
				WaitForSingleObjectEx(m_Handle, INFINITE, FALSE);
			}

			m_fenceValues[m_backBufferIndex] = currentFenceValue + 1;
		}

		uint32_t GetFrameCount() const {
			return FRAME_COUNT;
		}

		uint32_t GetFrameIndex() const {
			return pSwapChain->GetCurrentBackBufferIndex();
		}

		ID3D12Device9* pDevice;

		ID3D12CommandAllocator* pFrameAllocator[FRAME_COUNT];
		ID3D12GraphicsCommandList6* pList;

		IDXGISwapChain4* pSwapChain;

		ID3D12CommandQueue* pMainQueue;

		//fence
		ID3D12Fence1* pFence = nullptr;
		UINT64 m_fenceValues[3]{ 0,0,0 };
		UINT m_backBufferIndex = 0;
		HANDLE m_Handle = nullptr;
		// sc rtv
		ID3D12DescriptorHeap* p_SC_RTVHeap;
		UINT mRTVDescriptorSize;

		uint32_t mWidth;
		uint32_t mHeight;

		ID3D12PipelineState* pOutputPipeline;
		ID3D12RootSignature* pOutputRoot;
	};

	Device* CreateDevice(DeviceDesc* pDesc) {
		return new ADevice(pDesc);
	}

	struct ADescriptorHeapPool : public DescriptorHeapPool {
		ADescriptorHeapPool(DescriptorHeapPoolDesc* pDesc) {
			pDevice = pDesc->pDevice;
			if (!pDevice) {
				throw std::exception("Device is nullptr.");
			}

			D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
			HeapDesc.NumDescriptors = 10000;
			HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			pDevice->GetDevice()->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&pCBV_SRV_UAVHeap));

			HeapDesc.NumDescriptors = 100;
			HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			pDevice->GetDevice()->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&pSamplerHeap));
		}

		~ADescriptorHeapPool() {
			pCBV_SRV_UAVHeap->Release();
			pSamplerHeap->Release();
		}

		void Release() { delete this; }
		virtual ID3D12DescriptorHeap* GetCBV_SRV_UAVDescriptorHeap() const {
			return pCBV_SRV_UAVHeap;
		}
		virtual ID3D12DescriptorHeap* GetSamplerDescriptorHeap() const {
			return pSamplerHeap;
		}
		D3D12_CPU_DESCRIPTOR_HANDLE AllocCBV_SRV_UAVDescriptor() {
			auto pos = getfirstPosAndUpdate(mCBV_SRV_UAV);
			auto CBV_SRV_UAV_size = pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(pCBV_SRV_UAVHeap->GetCPUDescriptorHandleForHeapStart(), pos, CBV_SRV_UAV_size);
			return pCBV_SRV_UAVHeap->GetCPUDescriptorHandleForHeapStart();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE AllocSamplerDescriptor() {
			auto pos = getfirstPosAndUpdate(mSampler);
			auto sampler_size = pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(pSamplerHeap->GetCPUDescriptorHandleForHeapStart(), pos, sampler_size);
			return handle;
		}
		void FreeCBV_SRV_UAVDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE handle) {
			auto start = pCBV_SRV_UAVHeap->GetCPUDescriptorHandleForHeapStart();
			auto offset = handle.ptr - start.ptr;
			auto CBV_SRV_UAV_size = pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			auto index = offset / CBV_SRV_UAV_size;
			mCBV_SRV_UAV[index] = false;
		}

		void FreeSamplerDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE handle) {
			auto start = pSamplerHeap->GetCPUDescriptorHandleForHeapStart();
			auto offset = handle.ptr - start.ptr;
			auto sampler_size = pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
			auto index = offset / sampler_size;
			mSampler[index] = false;
		}

		size_t getfirstPosAndUpdate(std::vector<bool>& mVec) {
			size_t pos = mVec.size();

			for (size_t i = 0;i < pos; ++i) {
				if (!mVec[i]) {
					mVec[i] = true;
					return i;
				}
			}

			mVec.push_back(true);
			return pos;
		}

		Device* pDevice;
		ID3D12DescriptorHeap* pCBV_SRV_UAVHeap;
		ID3D12DescriptorHeap* pSamplerHeap;

		std::vector<bool> mCBV_SRV_UAV;
		std::vector<bool> mSampler;
	};
	DescriptorHeapPool* CreateDescriptorHeapPool(DescriptorHeapPoolDesc* pDesc) {
		return new ADescriptorHeapPool(pDesc);
	}

	struct AGBuffer : public GBuffer {
		AGBuffer(GBufferDesc* pDesc) {
			pDevice = pDesc->pDevice;
			pPool = pDesc->pPool;
			if (!pDevice) {
				throw std::exception("Device is nullptr.");
			}
			if (!pPool) {
				throw std::exception("DescriptorHeapPool is nullptr.");
			}

            auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, pDesc->mWidth, pDesc->mHeight);
			resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            pDevice->GetDevice()->CreateCommittedResource(
                &heapProp,
                D3D12_HEAP_FLAG_NONE,
                &resDesc,
				D3D12_RESOURCE_STATE_COMMON,
                nullptr,
                IID_PPV_ARGS(&pBaseColor));
			//rtv heap
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = 4;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			pDevice->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&pGBufferRTVDescriptorHeap));
			// base color rtv srv
			pDevice->GetDevice()->CreateRenderTargetView(pBaseColor, nullptr, pGBufferRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			mBaseColorSRV = pDesc->pPool->AllocCBV_SRV_UAVDescriptor();
			pDevice->GetDevice()->CreateShaderResourceView(pBaseColor, nullptr, mBaseColorSRV);

			mWidth = pDesc->mWidth;
			mHeight = pDesc->mHeight;
		}
		~AGBuffer() {
			pPool->FreeCBV_SRV_UAVDescriptor(mBaseColorSRV);
			pGBufferRTVDescriptorHeap->Release();
			pBaseColor->Release();
		}
		void Release() { delete this; }
		ID3D12Resource* GetBaseColorResource() const {
			return pBaseColor;
		}
		DXGI_FORMAT GetBaseColorFormat() const {
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetBaseColorRTV() const {
			return pGBufferRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		}

		uint32_t GetBufferWidth() const {
			return mWidth;
		}

		uint32_t GetBufferHeight() const {
			return mHeight;
		}

		Device* pDevice;
		DescriptorHeapPool* pPool;
		ID3D12DescriptorHeap* pGBufferRTVDescriptorHeap;
		ID3D12Resource* pBaseColor;
		D3D12_CPU_DESCRIPTOR_HANDLE mBaseColorSRV;
		uint32_t mWidth;
		uint32_t mHeight;
	};

	GBuffer* CreateGBuffer(GBufferDesc* pDesc) {
		return new AGBuffer(pDesc);
	}

	struct AGraphicsPipelineState : public GraphicsPipelineState {
		AGraphicsPipelineState(GraphicsPipelineStateDesc* pDesc) {
			pDevice = pDesc->pDevice;
			if (!pDevice) {
				throw std::exception("Device is nullptr.");
			}

			auto VS = ReadData(pDesc->mVertexShaderCSO.c_str());
			auto PS = ReadData(pDesc->mPixelShaderCSO.c_str());
			pDevice->GetDevice()->CreateRootSignature(0, VS.data(), VS.size(), IID_PPV_ARGS(&pOutputRoot));

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
			psoDesc.InputLayout = pDesc->mInputLayout;
			psoDesc.pRootSignature = pOutputRoot;
			psoDesc.VS = { VS.data(), VS.size() };
			psoDesc.PS = { PS.data(), PS.size() };
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.SampleMask = 0xffffffff;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = pDesc->mNumRenderTargets;
			for (int i = 0;i < 8; ++i) {
				psoDesc.RTVFormats[i] = pDesc->mRTVFormats[i];
			}
			psoDesc.SampleDesc.Count = 1;
			pDevice->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pOutputPipeline));
		}
		~AGraphicsPipelineState() {
			pOutputRoot->Release();
			pOutputPipeline->Release();

		}
		void Release() { delete this; }

		ID3D12PipelineState* GetPipelineState() const {
			return pOutputPipeline;
		}

		ID3D12RootSignature* GetRootSignature() const {
			return pOutputRoot;
		}

		Device* pDevice;
		ID3D12RootSignature* pOutputRoot;
		ID3D12PipelineState* pOutputPipeline;
	};

	GraphicsPipelineState* CreateGraphicsPipelineState(GraphicsPipelineStateDesc* pDesc) {
		return new AGraphicsPipelineState(pDesc);
	}

	struct ADynamicVertexBuffer : public DynamicVertexBuffer {
		ADynamicVertexBuffer(DynamicVertexBufferDesc* pDesc) {
			pDevice = pDesc->pDevice;
			if (!pDevice) {
				throw std::exception("Device is nullptr.");
			}

		}

		~ADynamicVertexBuffer() {}

		void Release() {
			delete this;
		}

		Device* pDevice;
	};
}