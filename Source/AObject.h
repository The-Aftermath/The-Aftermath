#pragma once

namespace TheAftermath {

	struct AObject {
		virtual void Release() = 0;
	};

	void RemoveObject(AObject* pObj);
}