#pragma once
#include <Windows.h>
#include <string>
#include <cstdint>
namespace TheAftermath {

	struct RenderWindowDesc {
		std::wstring mTitle; 
		uint32_t mWidth; 
		uint32_t mHeight;
		HINSTANCE mHinstance;
		WNDPROC pFunction;
	};

	class RenderWindow {
	public:
		virtual HWND GetHWND() const = 0;
		virtual void Show(int nCmdShow) = 0;
	};

	RenderWindow* CreateRenderWindow(RenderWindowDesc* pDesc);
	void RemoveRenderWindow(RenderWindow* pWindow);
	

}