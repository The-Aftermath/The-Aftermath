#include "Graphics.h"

namespace TheAftermath {


	struct ADevice : public Device {

		ADevice(DeviceDesc* pDesc) {


			mWidth = pDesc->mWidth;
			mHeight = pDesc->mHeight;
		}

		~ADevice() {

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
