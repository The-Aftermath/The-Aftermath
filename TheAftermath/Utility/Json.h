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
		JsonValue(const winrt::Windows::Data::Json::JsonValue& value) : mValue(value) {}
		JsonValue(const JsonValue&) = default;
		JsonValue(JsonValue&& r) noexcept : mValue(r.mValue) {
			r.mValue = winrt::Windows::Data::Json::JsonValue(nullptr);
		}

		JsonValue& operator=(const JsonValue&) = default;
		JsonValue& operator=(JsonValue&& r) noexcept {
			if (this != &r) {
				mValue = r.mValue;
				r.mValue = winrt::Windows::Data::Json::JsonValue(nullptr);
			}

			return *this;
		}

		~JsonValue() {}

		auto GetValue() const {
			return mValue;
		}

		bool Parse(const std::wstring& input) {
			return winrt::Windows::Data::Json::JsonValue::TryParse(input, mValue);
		}

		static auto CreateBooleanValue(bool input) {
			return JsonValue(winrt::Windows::Data::Json::JsonValue::CreateBooleanValue(input));
		}
		static auto CreateNumberValue(double input) {
			return JsonValue(winrt::Windows::Data::Json::JsonValue::CreateNumberValue(input));
		}
		static auto CreateStringValue(const std::wstring& input) {
			return JsonValue(winrt::Windows::Data::Json::JsonValue::CreateStringValue(input));
		}
		static auto CreateNullValue() {
			return JsonValue(winrt::Windows::Data::Json::JsonValue::CreateNullValue());
		}

		auto GetBoolean() const {
			return mValue.GetBoolean();
		}
		auto GetString() const {
			std::wstring temp{ mValue.GetString() };
			return temp;
		}
		auto GetNumber() const {
			return mValue.GetNumber();
		}
		auto GetArray() const {
			return mValue.GetArray();
		}
		auto GetObjectW() const {
			return mValue.GetObjectW();
		}

	private:
		winrt::Windows::Data::Json::JsonValue mValue;
	};

	class JsonArray {
	public:
		JsonArray() {}
		JsonArray(const JsonArray&) = default;
		JsonArray(JsonArray&& r) noexcept {
			mArray = r.mArray;
			r.mArray.Clear();
		}

		JsonArray& operator=(const JsonArray&) = default;
		JsonArray& operator=(JsonArray&& r) noexcept {
			if (this != &r) {
				mArray = r.mArray;
				r.mArray.Clear();
			}

			return *this;
		}

		~JsonArray() {}

		bool Parse(const std::wstring& input) {
			return winrt::Windows::Data::Json::JsonArray::TryParse(input, mArray);
		}

		void InsertAt(uint32_t index, const JsonValue &value) {
			mArray.InsertAt(index, value.GetValue());
		}

		std::wstring Stringify() {
			std::wstring temp_str{ mArray.Stringify() };
			return temp_str;
		}

	private:
		winrt::Windows::Data::Json::JsonArray mArray;
	};

	class JsonObject {
	public:
		JsonObject() {}
		JsonObject(const JsonObject&) = default;
		JsonObject(JsonObject&& r) noexcept {
			mObject = r.mObject;
			r.mObject.Clear();
		}

		JsonObject& operator=(const JsonObject&) = default;
		JsonObject& operator=(JsonObject&& r) noexcept {
			if (this != &r) {
				mObject = r.mObject;
				r.mObject.Clear();
			}

			return *this;
		}

		~JsonObject() {}

		bool Parse(const std::wstring& input) {
			return winrt::Windows::Data::Json::JsonObject::TryParse(input, mObject);
		}

		void Insert(const std::wstring &key, const JsonValue& value) {
			mObject.Insert(key, value.GetValue());
		}

		std::wstring Stringify() {
			std::wstring temp_str{ mObject.Stringify() };
			return temp_str;
		}

	private:
		winrt::Windows::Data::Json::JsonObject mObject;
	};
}
