#include <Windows.h>
#include <winnt.h>
#include <DirectXMath.h>

#include <winrt/base.h>

#include "Window/RenderWindow.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (!DirectX::XMVerifyCPUSupport()) {
        return 1;
    }

    winrt::init_apartment();

    TheAftermath::RenderWindow window;

    winrt::clear_factory_cache();
    winrt::uninit_apartment();
    return 0;
}