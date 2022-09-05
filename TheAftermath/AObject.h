#pragma once

namespace TheAftermath {

	struct AObject {
		virtual void Release() = 0;
	};

	inline void RemoveObject(AObject* pObj) {
		if (pObj) {
			pObj->Release();
		}
	}

}
