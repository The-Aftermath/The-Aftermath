#include <Windows.h>
#include "GameWindow.h"
#include "bgfx/bgfx.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void Draw() {

}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
    TheAftermath::GameWindowDesc gamewindowDesc;
    gamewindowDesc.mTitle = L"TheAftermath";
    gamewindowDesc.mCmdShow = nShowCmd;
    gamewindowDesc.mWidth = 800;
    gamewindowDesc.mHeight = 600;
    gamewindowDesc.mHinstance = hInstance;
    gamewindowDesc.pFunction = WndProc;
    auto gamewindow = TheAftermath::CreateGameWindow(&gamewindowDesc);


    gamewindow->Run(Draw);
    return 0;
}