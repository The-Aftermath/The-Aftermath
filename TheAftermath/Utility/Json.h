#pragma once
#include <winrt/windows.foundation.h>
#include <winrt/windows.foundation.collections.h>
#include <winrt/windows.data.json.h>
#include <string>
#include <cstdint>
namespace TheAftermath {

	class JsonValue {
	public:
		JsonValue() : mValue(nullptr) {}
	private:
		winrt::Windows::Data::Json::JsonValue mValue;
	};

	class JsonNode {
	public:
		JsonNode() {}
		JsonNode(const JsonNode&) = default;
		JsonNode(JsonNode&& r) noexcept {
			mArray = r.mArray;
			r.mArray.Clear();
			mObject = r.mObject;
			r.mObject.Clear();
		}

		JsonNode& operator=(const JsonNode&) = default;
		JsonNode& operator=(JsonNode&& r) noexcept {
			if (this != &r) {
				mArray = r.mArray;
				r.mArray.Clear();
				mObject = r.mObject;
				r.mObject.Clear();
			}

			return *this;
		}

		~JsonNode() {}

		bool Parse(const std::wstring& input) {
			bool ret = winrt::Windows::Data::Json::JsonArray::TryParse(input, mArray);
			if (!ret) {
				ret = winrt::Windows::Data::Json::JsonObject::TryParse(input, mObject);
			}
			return ret;
		}

		void InsertAt(uint32_t index, std::wstring const& value) {
			//mArray.InsertAt(index,);
		}
	private:
		winrt::Windows::Data::Json::JsonArray mArray;
		winrt::Windows::Data::Json::JsonObject mObject;
	};
}
