#pragma once
#include "AObject.h"

#include <Windows.h>
#include <cstdint>
#include <string>
#include <functional>
#include <utility>
namespace TheAftermath {

	struct GameWindowDesc {
		std::wstring mTitle;
		uint32_t mWidth = 800;
		uint32_t mHeight = 600;
		HINSTANCE mHinstance = NULL;
		int mCmdShow = SW_SHOWDEFAULT;
		WNDPROC pFunction = NULL;
	};

	struct GameWindow : public AObject {
		virtual HWND GetHWND() const = 0;
		virtual int GetCmdShow() const = 0;

		template <typename Function, typename... Args>
		void Run(Function&& f, Args&&... args) {
			ShowWindow(GetHWND(), GetCmdShow());
			MSG msg{};
			while (msg.message != WM_QUIT) {
				if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
					TranslateMessage(&msg);
					DispatchMessageW(&msg);
				}
				else {
					std::invoke(std::forward<Function>(f), std::forward<Args>(args)...);
				}
			}
		}
	};
	GameWindow* CreateGameWindow(GameWindowDesc* pDesc);
}
