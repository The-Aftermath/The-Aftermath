#pragma once
#include <Windows.h>

namespace TheAftermath {
    struct GraphicsDeviceDesc {
        HWND mHwnd;
    };

    class GraphicsDevice {
    public:
    };

    GraphicsDevice* CreateGraphicsDevice(GraphicsDeviceDesc* pDesc);

}
