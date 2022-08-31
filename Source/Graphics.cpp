#include "Graphics.h"
#include "Utility.h"
#include "d3dx12.h"
#include <combaseapi.h>
#include <winerror.h>
#include <exception>
#include <wrl.h>

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
		void Release() {
			delete this;
		}
		ID3D12Device* GetDevice() const {
			return pDevice;
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
}