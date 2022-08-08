#include <Windows.h>
#include <winnt.h>

#include <winrt/base.h>

#include "Context/Context.h"
#include "Window/RenderWindow.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (WM_DESTROY == message) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void RunLoop() {
    MSG msg = {};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {

        }
    }
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    winrt::init_apartment();
    TheAftermath::RenderWindowDesc windowDesc;
    windowDesc.mHinstance = hInstance;
    windowDesc.mTitle = L"TheAftermath";
    windowDesc.mWidth = 800;
    windowDesc.mHeight = 600;
    windowDesc.pFunction = WndProc;

    auto window = TheAftermath::CreateRenderWindow(&windowDesc);
    window->Show(nShowCmd);
    RunLoop();

    TheAftermath::RemoveRenderWindow(window);
    winrt::clear_factory_cache();
    winrt::uninit_apartment();
    return 0;
}