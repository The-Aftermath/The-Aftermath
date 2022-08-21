#include "Json.h"

namespace TheAftermath {

	JsonObject JsonArray::GetObjectAt(uint32_t i) const {
		auto obj = mArray.GetObjectAt(i);
		return JsonObject(std::wstring{ obj.Stringify() });
	}
}
