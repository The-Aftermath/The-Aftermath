#include "Device.h"
#include "bgfx/bgfx.h"
#include <combaseapi.h>
namespace TheAftermath {

	struct ADevice : public Device {

		ADevice(DeviceDesc* pDecs) {

			D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&pDevice));
			bgfx::Init init;
			init.type = bgfx::RendererType::Direct3D12;
			init.vendorId = BGFX_PCI_ID_NONE;
			init.platformData.nwh = pDecs->mHwnd;
			init.platformData.ndt = nullptr;
			//init.platformData.context = pDevice;
			init.resolution.width = pDecs->mWidth;
			init.resolution.height = pDecs->mHeight;
			init.resolution.reset = BGFX_RESET_VSYNC;
			bgfx::init(init);

		}

		~ADevice() {
			
			bgfx::shutdown();
			pDevice->Release();
		}

		void Release() {
			delete this;
		}

		void Present() {
			bgfx::frame();
		}

		ID3D12Device7* GetDevice() const {
			return nullptr;
		}

		ID3D12Device7* pDevice;
	};

	Device* CreateDevice(DeviceDesc* pDecs) {
		return new ADevice(pDecs);
	}
}
