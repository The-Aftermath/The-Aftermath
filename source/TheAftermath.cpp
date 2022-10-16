#include <Windows.h>

#include "GameScene.h"
#include "Device.h"
#include "GameRenderer.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProcW(hWnd, message, wParam, lParam);
}


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {

	UINT w = 800, h = 600;

	WNDCLASSEXW wcex{};
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIconW(hInstance, L"IDI_ICON");
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wcex.lpszClassName = L"The Aftermath";
	wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");

	RegisterClassExW(&wcex);
	auto stype = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;
	RECT rc = { 0, 0, static_cast<LONG>(w), static_cast<LONG>(h) };
	AdjustWindowRect(&rc, stype, FALSE);
	auto hwnd = CreateWindowExW(0, L"The Aftermath", L"The Aftermath", stype, 40, 40, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, wcex.hInstance, NULL);

	//GameRenderer renderer;
	//renderer.init(hwnd, w, h);
	Device device(hwnd, w, h);

	::ShowWindow(hwnd, nShowCmd);
	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else
		{
			//renderer.update();
			device.present();
		}
	}

	//renderer.destroy();

	return 0;
}