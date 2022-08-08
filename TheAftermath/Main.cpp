#include <Windows.h>
#include <winnt.h>

#include <winrt/base.h>

#include "Window/RenderWindow.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    winrt::init_apartment();



    winrt::clear_factory_cache();
    winrt::uninit_apartment();
    return 0;
}