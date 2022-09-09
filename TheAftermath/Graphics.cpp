#include "Graphics.h"
#include "bgfx/c99/bgfx.h"
namespace TheAftermath {


	struct ADevice : public Device {

		ADevice(DeviceDesc* pDecs) {
			bgfx_init_t init;
			bgfx_init_ctor(&init);
			init.type = BGFX_RENDERER_TYPE_DIRECT3D12;
			init.vendorId = BGFX_PCI_ID_NONE;
			init.platformData.nwh = pDecs->mHwnd;
			init.resolution.width = pDecs->mWidth;
			init.resolution.height = pDecs->mHeight;
			init.resolution.reset = BGFX_RESET_VSYNC;

			bgfx_init(&init);
			auto data = bgfx_get_internal_data();
			pDevice = (ID3D12Device7*)data->context;

			mWidth = pDecs->mWidth;
			mHeight = pDecs->mHeight;

			bgfx_set_view_clear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
		}

		~ADevice() {
			bgfx_shutdown();
		}

		void Release() {
			delete this;
		}

		void Present() {
			bgfx_set_view_rect(0, 0, 0, uint16_t(mWidth), uint16_t(mHeight));
			bgfx_touch(0);
			bgfx_frame(false);
		}

		ID3D12Device7* GetDevice() const {
			return pDevice;
		}

		ID3D12Device7* pDevice;

		uint32_t mWidth;
		uint32_t mHeight;
	};

	Device* CreateDevice(DeviceDesc* pDecs) {
		return new ADevice(pDecs);
	}
}
