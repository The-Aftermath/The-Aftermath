#include "Device.h"
#include "bgfx/bgfx.h"
#include <combaseapi.h>
namespace TheAftermath {

	struct ADevice : public Device {

		ADevice(DeviceDesc* pDecs) {


			//bgfx::init(init);


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

	};

	Device* CreateDevice(DeviceDesc* pDecs) {
		return new ADevice(pDecs);
	}
}
