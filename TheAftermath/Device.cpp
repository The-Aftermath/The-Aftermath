#include "Device.h"
#include "bgfx/bgfx.h"
#include <combaseapi.h>
#include <thread>
namespace TheAftermath {

	void func() {
		ID3D12Device7* pDevice;
		if (SUCCEEDED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&pDevice)))) {
			pDevice->Release();
			OutputDebugStringW(L"sss\n");
		}
		else {
			OutputDebugStringW(L"error\n");
		}
	}

	struct ADevice : public Device {

		ADevice(DeviceDesc* pDecs) {


			D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&pDevice));
			//bgfx::Init init;
			//init.type = bgfx::RendererType::Direct3D12;
			//init.vendorId = BGFX_PCI_ID_NONE;
			//init.platformData.nwh = pDecs->mHwnd;
			//init.resolution.width = pDecs->mWidth;
			//init.resolution.height = pDecs->mHeight;
			//init.resolution.reset = BGFX_RESET_VSYNC;
			//bgfx::init(init);
			std::thread myNewThread(func);
			myNewThread.join();

		}

		~ADevice() {
			
			//bgfx::shutdown();
			//pDevice->Release();
		}

		void Release() {
			delete this;
		}

		void Present() {
			//bgfx::frame();
		}

		ID3D12Device7* GetDevice() const {
			return nullptr;
		}

		ID3D12Device7* pDevice;
		ID3D12Device7* pDevice1;
		ID3D12Device7* pDevice2;
	};

	Device* CreateDevice(DeviceDesc* pDecs) {
		return new ADevice(pDecs);
	}
}
