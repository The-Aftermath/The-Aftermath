#include "AObject.h"
namespace TheAftermath {
	void RemoveObject(AObject* pObj) {
		if (pObj) {
			pObj->Release();
		}
		
	}
}