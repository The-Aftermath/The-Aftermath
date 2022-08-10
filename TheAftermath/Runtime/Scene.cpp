#include "Scene.h"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <system_error>
#include <vector>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <combaseapi.h>
#include "d3dx12.h"

namespace TheAftermath {

    inline std::vector<uint8_t> ReadData(_In_z_ const wchar_t* name)
    {
        std::ifstream inFile(name, std::ios::in | std::ios::binary | std::ios::ate);

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
        if (!inFile)
        {
            wchar_t moduleName[_MAX_PATH] = {};
            if (!GetModuleFileNameW(nullptr, moduleName, _MAX_PATH))
                throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "GetModuleFileNameW");

            wchar_t drive[_MAX_DRIVE];
            wchar_t path[_MAX_PATH];

            if (_wsplitpath_s(moduleName, drive, _MAX_DRIVE, path, _MAX_PATH, nullptr, 0, nullptr, 0))
                throw std::runtime_error("_wsplitpath_s");

            wchar_t filename[_MAX_PATH];
            if (_wmakepath_s(filename, _MAX_PATH, drive, path, name, nullptr))
                throw std::runtime_error("_wmakepath_s");

            inFile.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
        }
#endif

        if (!inFile)
            throw std::runtime_error("ReadData");

        const std::streampos len = inFile.tellg();
        if (!inFile)
            throw std::runtime_error("ReadData");

        std::vector<uint8_t> blob;
        blob.resize(size_t(len));

        inFile.seekg(0, std::ios::beg);
        if (!inFile)
            throw std::runtime_error("ReadData");

        inFile.read(reinterpret_cast<char*>(blob.data()), len);
        if (!inFile)
            throw std::runtime_error("ReadData");

        inFile.close();

        return blob;
    }

	class AScene : public Scene {
	public:
		AScene(SceneDesc* pDesc) {
			pDevice = pDesc->pDevice;

            auto SceneVSBlob = ReadData(L"SceneVS.cso");
            auto ScenePSBlob = ReadData(L"ScenePS.cso");
            pDevice->GetDevice()->CreateRootSignature(0, SceneVSBlob.data(), SceneVSBlob.size(), IID_PPV_ARGS(&pSceneRoot));

            D3D12_INPUT_ELEMENT_DESC sceneInputDesc[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            };

            D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
            psoDesc.InputLayout = { sceneInputDesc, 3 };
            psoDesc.pRootSignature = pSceneRoot;
            psoDesc.VS = { SceneVSBlob.data(), SceneVSBlob.size() };
            psoDesc.PS = { ScenePSBlob.data(), ScenePSBlob.size() };
            psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            psoDesc.SampleMask = 0xffffffff;
            psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            psoDesc.NumRenderTargets = 1;
            psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
            psoDesc.SampleDesc.Count = 1;
            pDevice->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pScenePSO));

            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
            rtvHeapDesc.NumDescriptors = 3;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            pDevice->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&p_SC_RTVHeap));
            mRTVDescriptorSize = pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(p_SC_RTVHeap->GetCPUDescriptorHandleForHeapStart());
            for (UINT n = 0; n < 3; n++)
            {
                ID3D12Resource* _res = nullptr;
                pDevice->GetSwapChain()->GetBuffer(n, IID_PPV_ARGS(&_res));
                pDevice->GetDevice()->CreateRenderTargetView(_res, nullptr, rtvHandle);
                rtvHandle.Offset(1, mRTVDescriptorSize);
                _res->Release();

                pDevice->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pFrameAllocator[n]));
            }
            _CreateCmdList();
            
		}
        ~AScene() {
            pDevice->Wait();

            pSceneRoot->Release();
            pScenePSO->Release();
            p_SC_RTVHeap->Release();

            pFrameAllocator[0]->Release();
            pFrameAllocator[1]->Release();
            pFrameAllocator[2]->Release();
            pList->Release();
        }

        void Update() {
            

            pDevice->Present();
        }

        void _CreateCmdList() {
            ID3D12Device11* device;
            pDevice->GetDevice()->QueryInterface(&device);
            device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pList));
            device->Release();
        }

        GraphicsDevice* pDevice;

        ID3D12RootSignature* pSceneRoot;
        ID3D12PipelineState* pScenePSO;
        ID3D12DescriptorHeap* p_SC_RTVHeap;
        UINT mRTVDescriptorSize;

        ID3D12CommandAllocator* pFrameAllocator[3];
        ID3D12GraphicsCommandList8* pList;
	};

	Scene* CreateScene(SceneDesc* pDesc) {
		return new AScene(pDesc);
	}
	void RemoveScene(Scene* pScene) {
		auto temp_ptr = dynamic_cast<AScene*>(pScene);
		delete temp_ptr;
	}
}