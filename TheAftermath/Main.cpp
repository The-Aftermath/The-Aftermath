#include <Windows.h>

#include "Utility/Utility.h"
#include "Utility/Json.h"
#include "Context/Context.h"
#include "Window/RenderWindow.h"
#include "Graphics/Graphics.h"
#include "Runtime/Scene.h"
#include "Runtime/Camera.h"


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void Draw(TheAftermath::Scene *pScene) {
    pScene->Update();
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    TheAftermath::InitContext();
    TheAftermath::RenderWindowDesc windowDesc;
    windowDesc.mHinstance = hInstance;
    windowDesc.mTitle = L"TheAftermath";
    windowDesc.mWidth = 800;
    windowDesc.mHeight = 600;
    windowDesc.pFunction = WndProc;
    windowDesc.pParam = NULL;
    auto window = TheAftermath::CreateRenderWindow(&windowDesc);

    TheAftermath::GraphicsDeviceDesc deviceDesc;
    deviceDesc.mWidth = 800;
    deviceDesc.mHeight = 600;
    deviceDesc.mHwnd = window->GetHWND();
    auto device = TheAftermath::CreateGraphicsDevice(&deviceDesc);

    TheAftermath::SceneDesc sceneDesc;
    sceneDesc.pDevice = device;
    sceneDesc.mLight = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
    auto scene = TheAftermath::CreateScene(&sceneDesc);


    scene->LoadStaticModel(L"Model/Box");
    window->Show(nShowCmd);
    TheAftermath::RunLoop(Draw, scene);

    TheAftermath::RemoveScene(scene);
    TheAftermath::RemoveGraphicsDevice(device);
    TheAftermath::RemoveRenderWindow(window);
    TheAftermath::RemoveContext();
    return 0;
}