#include <Windows.h>
#include "AObject.h"
#include "GameWindow.h"
#include "Device.h"
#include "Scene.h"
#include "Vertex.h"
#include "CommandBuffer.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void Draw(TheAftermath::Scene* pScene) {
    pScene->Update();
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
    constexpr unsigned width = 800, height = 600;
    TheAftermath::GameWindowDesc gamewindowDesc;
    gamewindowDesc.mTitle = L"TheAftermath";
    gamewindowDesc.mCmdShow = nShowCmd;
    gamewindowDesc.mWidth = width;
    gamewindowDesc.mHeight = height;
    gamewindowDesc.mHinstance = hInstance;
    gamewindowDesc.pFunction = WndProc;
    auto gamewindow = TheAftermath::CreateGameWindow(&gamewindowDesc);
    TheAftermath::DeviceDesc deviceDesc;
    deviceDesc.mHwnd = gamewindow->GetHWND();
    deviceDesc.mWidth = width;
    deviceDesc.mHeight = height;
    auto device = TheAftermath::CreateDevice(&deviceDesc);

    TheAftermath::SceneDesc sceneDesc;
    sceneDesc.pDevice = device;
    auto scene = TheAftermath::CreateScene(&sceneDesc);

    gamewindow->Run(Draw, scene);

    TheAftermath::RemoveObject(scene);
    TheAftermath::RemoveObject(device);
    TheAftermath::RemoveObject(gamewindow);
    return 0;
}