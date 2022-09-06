#include "Device.h"
#include "bgfx/bgfx.h"
namespace TheAftermath {

	struct ADevice : public Device {

		ADevice(DeviceDesc* pDecs) {
			bgfx::Init init;
			init.type = bgfx::RendererType::Direct3D12;
			init.vendorId = BGFX_PCI_ID_NONE;
			init.platformData.nwh = pDecs->mHwnd;
			init.platformData.ndt = nullptr;
			init.resolution.width = pDecs->mWidth;
			init.resolution.height = pDecs->mHeight;
			init.resolution.reset = BGFX_RESET_VSYNC;
			bgfx::init(init);
		}

		~ADevice() {
			bgfx::shutdown();
		}

		void Release() {
			
		}

		ID3D12Device7* GetDevice() const {
			return nullptr;
		}
	};

	Device* CreateDevice(DeviceDesc* pDecs) {
		return new ADevice(pDecs);
	}
}
