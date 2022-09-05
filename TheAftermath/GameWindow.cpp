#include "GameWindow.h"
#include <exception>
namespace TheAftermath {
	class AGameWindow : public GameWindow {
	public:
		AGameWindow(GameWindowDesc* pDesc) {

			if (!pDesc || pDesc->mTitle.empty() || !pDesc->mHinstance || !pDesc->pFunction) {
				throw std::exception("Invalid argument");
			}

			WNDCLASSEXW wcex{};
			wcex.cbSize = sizeof(WNDCLASSEXW);
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = pDesc->pFunction;
			wcex.hInstance = pDesc->mHinstance;
			wcex.hIcon = LoadIconW(pDesc->mHinstance, L"IDI_ICON");
			wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
			wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
			wcex.lpszClassName = L"TheAftermath";
			wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");

			RegisterClassExW(&wcex);
			auto stype = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;
			RECT rc = { 0, 0, static_cast<LONG>(pDesc->mWidth), static_cast<LONG>(pDesc->mHeight) };
			AdjustWindowRect(&rc, stype, FALSE);
			mHwnd = CreateWindowExW(0, L"TheAftermath", pDesc->mTitle.c_str(), stype,
				CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, wcex.hInstance,
				NULL);
			show = pDesc->mCmdShow;
		}

		void Release() {
			delete this;
		}

		HWND GetHWND() const {
			return mHwnd;
		}

		int GetCmdShow() const {
			return show;
		}

		HWND mHwnd;
		int show;
	};

	GameWindow* CreateGameWindow(GameWindowDesc* pDesc) {
		return new AGameWindow(pDesc);
	}
}