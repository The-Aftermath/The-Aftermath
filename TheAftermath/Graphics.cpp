#include "Graphics.h"
#include "bgfx/c99/bgfx.h"
namespace TheAftermath {


	struct ADevice : public Device {

		ADevice(DeviceDesc* pDesc) {
			bgfx_init_t init;
			bgfx_init_ctor(&init);
#ifdef _DEBUG
			init.debug = true;
#endif // _DEBUG
			init.type = BGFX_RENDERER_TYPE_DIRECT3D12;
			init.vendorId = BGFX_PCI_ID_NONE;
			init.platformData.nwh = pDesc->mHwnd;
			init.resolution.width = pDesc->mWidth;
			init.resolution.height = pDesc->mHeight;
			init.resolution.reset = BGFX_RESET_VSYNC;

			bgfx_init(&init);
			auto data = bgfx_get_internal_data();
			pDevice = (ID3D12Device7*)data->context;

			mWidth = pDesc->mWidth;
			mHeight = pDesc->mHeight;
		}

		~ADevice() {
			bgfx_shutdown();
		}

		void Release() {
			delete this;
		}

		ID3D12Device7* GetDevice() const {
			return pDevice;
		}

		ID3D12Device7* pDevice;

		uint32_t mWidth;
		uint32_t mHeight;
	};

	Device* CreateDevice(DeviceDesc* pDesc) {
		return new ADevice(pDesc);
	}
}
