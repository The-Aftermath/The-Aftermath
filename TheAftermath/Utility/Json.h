#pragma once
#include <winrt/windows.foundation.h>
#include <winrt/windows.foundation.collections.h>
#include <winrt/windows.data.json.h>
#include <string>
#include <type_traits>
#include <cstddef>
#include <cstdint>

namespace TheAftermath {

	class JsonValue {
	public:
		JsonValue(std::nullptr_t) : mValue(nullptr) {}
		template<typename T>
			requires std::is_arithmetic_v<T>
		JsonValue(T t) : mValue(winrt::Windows::Data::Json::JsonValue::Parse(std::to_wstring(t))) {}
		JsonValue(const std::wstring& input) : mValue(winrt::Windows::Data::Json::JsonValue::CreateStringValue(input)) {}
		JsonValue(bool boolean) : mValue(winrt::Windows::Data::Json::JsonValue::CreateBooleanValue(boolean)) {}

	private:
		winrt::Windows::Data::Json::JsonValue mValue;
	};

	class JsonObject;

	class JsonArray {
	public:
		JsonArray() {}
		JsonArray(const std::wstring& input) {
			mArray = winrt::Windows::Data::Json::JsonArray::Parse(input);
		}

		double GetNumberAt(uint32_t i) const {
			return mArray.GetNumberAt(i);
		}
		bool GetBooleanAt(uint32_t i) const {
			return mArray.GetBooleanAt(i);
		}

		std::wstring GetStringAt(uint32_t i) const {
			auto str = mArray.GetStringAt(i);
			return std::wstring{ str };
		}

		JsonObject GetObjectAt(uint32_t i) const;
		JsonArray GetArrayAt(uint32_t i) const {
			auto arr = mArray.GetArrayAt(i);
			return JsonArray(std::wstring{ arr.Stringify() });
		}
	private:
		winrt::Windows::Data::Json::JsonArray mArray;
	};

	class JsonObject {
	public:
		JsonObject() {}
		JsonObject(const std::wstring& input) {
			mObject = winrt::Windows::Data::Json::JsonObject::Parse(input);
		}

		JsonObject GetNamedObject(const std::wstring& name) const {
			auto obj = mObject.GetNamedObject(name);
			return JsonObject(std::wstring{ obj.Stringify() });
		}
		std::wstring GetNamedString(const std::wstring& name) const {
			auto str = mObject.GetNamedString(name);
			return std::wstring{ str };
		}
		JsonArray GetNamedArray(const std::wstring& name) const {
			auto arr = mObject.GetNamedArray(name);
			return JsonArray(std::wstring{ arr.Stringify() });
		}

		bool GetNamedBoolean(const std::wstring& name) const {
			return mObject.GetNamedBoolean(name);
		}

		double GetNamedNumber(const std::wstring& name) const {
			return mObject.GetNamedNumber(name);
		}

	private:
		winrt::Windows::Data::Json::JsonObject mObject;
	};
}
