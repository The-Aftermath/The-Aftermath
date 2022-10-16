#pragma once
#include "nvrhi/nvrhi.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
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

	void _destroy() {
		mDeviceHandle->waitForIdle();
		mDeviceHandle->runGarbageCollection();

		_wait(mGFenceValue, pGraphicsQueue, pGFence, mGFenceEvent);
		_wait(mCopyFenceValue, pCopyQueue, pCopyFence, mCopyFenceEvent);
		_wait(mComputeFenceValue, pComputeQueue, pComputeFence, mComputeFenceEvent);

		CloseHandle(mComputeFenceEvent);
		pComputeFence->Release();
		CloseHandle(mCopyFenceEvent);
		pCopyFence->Release();
		CloseHandle(mGFenceEvent);
		pGFence->Release();
		pSwapChain->Release();
		pComputeQueue->Release();
		pCopyQueue->Release();
		pGraphicsQueue->Release();
		pDevice->Release();
	}

public:
	Device(HWND hwnd, UINT w, UINT h);
	~Device() {
		if (pDevice) {
			_destroy();
			pDevice = nullptr;
		}
	}

	Device(const Device&) = delete;
	Device& operator=(const Device&) = delete;

	Device(Device&&r) noexcept {
		r.pDevice->QueryInterface(&pDevice);
		r.pDevice->Release();
		r.pDevice = nullptr;

		r.pSwapChain->QueryInterface(&pSwapChain);
		r.pSwapChain->Release();
		r.pSwapChain = nullptr;

		r.pGraphicsQueue->QueryInterface(&pGraphicsQueue);
		r.pGraphicsQueue->Release();
		r.pGraphicsQueue = nullptr;

		r.pCopyQueue->QueryInterface(&pCopyQueue);
		r.pCopyQueue->Release();
		r.pCopyQueue = nullptr;

		r.pComputeQueue->QueryInterface(&pComputeQueue);
		r.pComputeQueue->Release();
		r.pComputeQueue = nullptr;

		r.pGFence->QueryInterface(&pGFence);
		r.pGFence->Release();
		r.pGFence = nullptr;

		r.pCopyFence->QueryInterface(&pCopyFence);
		r.pCopyFence->Release();
		r.pCopyFence = nullptr;

		r.pComputeFence->QueryInterface(&pComputeFence);
		r.pComputeFence->Release();
		r.pComputeFence = nullptr;

		mGFenceValue = r.mGFenceValue;
		mCopyFenceValue = r.mCopyFenceValue;
		mComputeFenceValue = r.mComputeFenceValue;

		mGFenceEvent = r.mGFenceEvent;
		mCopyFenceEvent = r.mCopyFenceEvent;
		mComputeFenceEvent = r.mComputeFenceEvent;

		mDeviceHandle = r.mDeviceHandle;
	}

	Device& operator=(Device&& r) noexcept {
		if (this != &r) {
			r.pDevice->QueryInterface(&pDevice);
			r.pDevice->Release();
			r.pDevice = nullptr;

			r.pSwapChain->QueryInterface(&pSwapChain);
			r.pSwapChain->Release();
			r.pSwapChain = nullptr;

			r.pGraphicsQueue->QueryInterface(&pGraphicsQueue);
			r.pGraphicsQueue->Release();
			r.pGraphicsQueue = nullptr;

			r.pCopyQueue->QueryInterface(&pCopyQueue);
			r.pCopyQueue->Release();
			r.pCopyQueue = nullptr;

			r.pComputeQueue->QueryInterface(&pComputeQueue);
			r.pComputeQueue->Release();
			r.pComputeQueue = nullptr;

			r.pGFence->QueryInterface(&pGFence);
			r.pGFence->Release();
			r.pGFence = nullptr;

			r.pCopyFence->QueryInterface(&pCopyFence);
			r.pCopyFence->Release();
			r.pCopyFence = nullptr;

			r.pComputeFence->QueryInterface(&pComputeFence);
			r.pComputeFence->Release();
			r.pComputeFence = nullptr;

			mGFenceValue = r.mGFenceValue;
			mCopyFenceValue = r.mCopyFenceValue;
			mComputeFenceValue = r.mComputeFenceValue;

			mGFenceEvent = r.mGFenceEvent;
			mCopyFenceEvent = r.mCopyFenceEvent;
			mComputeFenceEvent = r.mComputeFenceEvent;

			mDeviceHandle = r.mDeviceHandle;
		}
		return *this;
	}

	ID3D12Device* getDevice() const {
		return pDevice;
	}
	nvrhi::DeviceHandle getDeviceHandle() const {
		if (pDevice) {
			return mDeviceHandle;
		}
		return nullptr;
	}

	void present() {
		pSwapChain->Present(1, 0);
		_wait(mGFenceValue, pGraphicsQueue, pGFence, mGFenceEvent);
		_wait(mCopyFenceValue, pCopyQueue, pCopyFence, mCopyFenceEvent);
		_wait(mComputeFenceValue, pComputeQueue, pComputeFence, mComputeFenceEvent);

		mDeviceHandle->runGarbageCollection();
	}
};