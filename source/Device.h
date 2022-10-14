#pragma once
#include "nvrhi/nvrhi.h"
#include <d3d12.h>
#include <dxgi1_6.h>
class Device {
	ID3D12Device* pDevice;
	IDXGISwapChain4* pSwapChain;
	nvrhi::DeviceHandle mDeviceHandle;

	ID3D12CommandQueue* pGraphicsQueue;
	ID3D12CommandQueue* pCopyQueue;
	ID3D12CommandQueue* pComputeQueue;

	UINT64 mGFenceValue;
	ID3D12Fence* pGFence;
	HANDLE mGFenceEvent;
	UINT64 mCopyFenceValue;
	ID3D12Fence* pCopyFence;
	HANDLE mCopyFenceEvent;
	UINT64 mComputeFenceValue;
	ID3D12Fence* pComputeFence;
	HANDLE mComputeFenceEvent;

	void _wait(UINT64& fenceValue, ID3D12CommandQueue* pQueue, ID3D12Fence* pFence, HANDLE fenceEvent) {
		const UINT64 fence = fenceValue;
		pQueue->Signal(pFence, fence);
		fenceValue++;

		if (pFence->GetCompletedValue() < fence)
		{
			pFence->SetEventOnCompletion(fence, fenceEvent);
			WaitForSingleObject(fenceEvent, INFINITE);
		}
	}

public:
	Device(HWND hwnd, UINT w, UINT h);
	~Device();

	ID3D12Device* getDevice() const {
		return pDevice;
	}
	nvrhi::DeviceHandle getDeviceHandle() const {
		return mDeviceHandle;
	}

	void present() {
		pSwapChain->Present(1, 0);
		_wait(mGFenceValue, pGraphicsQueue, pGFence, mGFenceEvent);
		_wait(mCopyFenceValue, pCopyQueue, pCopyFence, mCopyFenceEvent);
		_wait(mComputeFenceValue, pComputeQueue, pComputeFence, mComputeFenceEvent);

		mDeviceHandle->runGarbageCollection();
	}
};