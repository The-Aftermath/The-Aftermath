#include "Graphics.h"
#include "bgfx/c99/bgfx.h"
namespace TheAftermath {


	struct ADevice : public Device {

		ADevice(DeviceDesc* pDecs) {
		}

		~ADevice() {

		}

		void Release() {
			delete this;
		}

		void Present() {

		}

		ID3D12Device7* GetDevice() const {
			return nullptr;
		}

	};

	Device* CreateDevice(DeviceDesc* pDecs) {
		return new ADevice(pDecs);
	}
}
