#pragma once
#include <Windows.h>
#include <string>
#include <string_view>
#include <cstdint>
namespace TheAftermath {

	class RenderWindow {
	public:
		RenderWindow() { mWidth = 800; mHeight = 600; }
		RenderWindow(const std::wstring_view v, uint32_t w, uint32_t h) :mTitle(v), mWidth(w), mHeight(h) {}
		RenderWindow(const RenderWindow&) = delete;
		RenderWindow(RenderWindow&&) noexcept = default;
		~RenderWindow() {}

		RenderWindow& operator=(const RenderWindow&) = delete;
		RenderWindow& operator=(RenderWindow&&) noexcept = default;

	private:
		std::wstring mTitle;
		uint32_t mWidth;
		uint32_t mHeight;
	};
}