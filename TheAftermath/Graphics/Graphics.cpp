#include "Graphics.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <combaseapi.h>
extern "C"
{
    __declspec(dllexport) extern const UINT D3D12SDKVersion = D3D12_SDK_VERSION;
    __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\";
}

namespace TheAftermath {

    class AGraphicsDevice : public GraphicsDevice {
    public:
        AGraphicsDevice(GraphicsDeviceDesc* pDesc) {
            ID3D12Debug* debugController = nullptr;
#if defined(_DEBUG)

            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
            {
                debugController->EnableDebugLayer();
            }
#endif
            D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&pDevice));
            
            if (debugController) {
                debugController->Release();
            }
        }
        ~AGraphicsDevice() {
            pDevice->Release();
        }

        ID3D12Device11* pDevice;
   
    };

    GraphicsDevice* CreateGraphicsDevice(GraphicsDeviceDesc* pDesc) {
        return new AGraphicsDevice(pDesc);
    }

}
