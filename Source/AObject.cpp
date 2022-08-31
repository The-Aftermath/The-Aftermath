#include "AObject.h"
namespace TheAftermath {
	void RemoveObject(AObject* pObj) {
		pObj->Release();
	}
}