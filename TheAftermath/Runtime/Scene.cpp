#include "Scene.h"
#include "Camera.h"
#include "../Utility/Json.h"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <system_error>
#include <vector>

#include <string>
#include <cstring>
#include <filesystem>

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
            mCBV_SRV_UVADescriptorSize = pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(p_SC_RTVHeap->GetCPUDescriptorHandleForHeapStart());
            for (UINT n = 0; n < 3; n++)
            {
                ID3D12Resource* _res = pDevice->GetResource(n);
                pDevice->GetDevice()->CreateRenderTargetView(_res, nullptr, rtvHandle);
                rtvHandle.Offset(1, mRTVDescriptorSize);

                pDevice->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pFrameAllocator[n]));
            }
            _CreateCmdList();
            _GetScreenSize();

            D3D12_DESCRIPTOR_HEAP_DESC sceneHeapDesc = {};
            sceneHeapDesc.NumDescriptors = 1000;
            sceneHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            sceneHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            pDevice->GetDevice()->CreateDescriptorHeap(&sceneHeapDesc, IID_PPV_ARGS(&pSceneHeapPool));

            UINT BUFFERSIZE = sizeof(SceneConstantBuffer);
            auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(BUFFERSIZE);
            pDevice->GetDevice()->CreateCommittedResource(
                &heapProp,
                D3D12_HEAP_FLAG_NONE,
                &resDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&pConstantBuffer));

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
            cbvDesc.BufferLocation = pConstantBuffer->GetGPUVirtualAddress();
            cbvDesc.SizeInBytes = BUFFERSIZE;
            pDevice->GetDevice()->CreateConstantBufferView(&cbvDesc, pSceneHeapPool->GetCPUDescriptorHandleForHeapStart());

            CD3DX12_RANGE readRange(0, 0);
            pConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pCbvDataBegin));
            sceneCB = (SceneConstantBuffer*)pCbvDataBegin;
            sceneCB->light = pDesc->mLight;
            pCamera = new Camera(
                0.f, 0.f, 0.f,
                0.f, 0.f, 1.f,
                0.f, 1.f, 0.f,
                0.785398163f, (float)mWidth / (float)mHeight, 0.1f, 1000.f
            );
            sceneCB->mvp = pCamera->GetVP();

            mTextureDescriptorHandle = pSceneHeapPool->GetCPUDescriptorHandleForHeapStart();
            mTextureDescriptorHandle.Offset(1, mCBV_SRV_UVADescriptorSize);
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

            pSceneHeapPool->Release();
            pConstantBuffer->Unmap(0, nullptr);
            pConstantBuffer->Release();

            delete pCamera;
        }

        void LoadStaticModel(const wchar_t* path) {
            std::filesystem::path modelParentPath{ path };
            winrt::Windows::Data::Json::JsonValue mValue(nullptr);
            JsonValue v(false);
            auto modelStem = modelParentPath.stem();
            auto modelJson = modelStem;
            modelJson.concat(".json");
            auto modelJsonPath = modelParentPath / modelJson;

            auto model_Json_file_size = std::filesystem::file_size(modelJsonPath);
            std::wifstream model_Json_fs(modelJsonPath);
            wchar_t* jsonStr = new wchar_t[model_Json_file_size + 1] {};
            model_Json_fs.read(jsonStr, model_Json_file_size);

            auto modelBin = modelStem;
            modelBin.concat(".bin");
            auto modelBinPath = modelParentPath / modelBin;

            auto model_Bin_file_size = std::filesystem::file_size(modelBinPath);
            std::wifstream model_Bin_fs(modelBinPath);
            wchar_t* binStr = new wchar_t[model_Bin_file_size + 1] {};
            model_Bin_fs.read(binStr, model_Bin_file_size);

            JsonObject jsonObj(jsonStr);

            auto meshCountStr = jsonObj.GetNamedString(L"MeshCount");
            auto meshCount = std::stoi(meshCountStr);

            JsonArray meshAttributes = jsonObj.GetNamedArray(L"MeshAttributes");

            for (int meshIndex = 0; meshIndex < meshCount; ++meshIndex) {
                auto meshData = meshAttributes.GetObjectAt(meshIndex);

                auto vertexOffsetStr = meshData.GetNamedString(L"VertexOffset");
                auto vertexOffset = std::stoi(vertexOffsetStr);

            }
            delete[]jsonStr;
            delete[]binStr;
        }

        void Update() {
            auto frameIndex = pDevice->GetFrameIndex();
            pFrameAllocator[frameIndex]->Reset();
            pList->Reset(pFrameAllocator[frameIndex], pScenePSO);
            auto renderTarget = pDevice->GetResource(frameIndex);

            pList->SetGraphicsRootSignature(pSceneRoot);

            ID3D12DescriptorHeap* ppHeaps[] = { pSceneHeapPool };
            pList->SetDescriptorHeaps(1, ppHeaps);
            pList->SetGraphicsRootConstantBufferView(0, pConstantBuffer->GetGPUVirtualAddress());

            CD3DX12_VIEWPORT viewport(0.F, 0.F, (FLOAT)mWidth, (FLOAT)mHeight);
            pList->RSSetViewports(1, &viewport);
            CD3DX12_RECT scissorRect(0, 0, mWidth, mHeight);
            pList->RSSetScissorRects(1, &scissorRect);

            auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
            pList->ResourceBarrier(1, &barrier);

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(p_SC_RTVHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, mRTVDescriptorSize);
            pList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
            const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
            pList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

            barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            pList->ResourceBarrier(1, &barrier);
            pList->Close();

            ID3D12CommandList* pLists[] = { pList };
            pDevice->GetImmediateCommandQueue()->ExecuteCommandLists(1, pLists);
            pDevice->Present();
        }

        void _CreateCmdList() {
            ID3D12Device11* device;
            pDevice->GetDevice()->QueryInterface(&device);
            device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pList));
            device->Release();
        }

        void _GetScreenSize() {
            DXGI_SWAP_CHAIN_DESC desc{};
            pDevice->GetSwapChain()->GetDesc(&desc);
            mWidth = desc.BufferDesc.Width;
            mHeight = desc.BufferDesc.Height;
        }

        GraphicsDevice* pDevice;

        ID3D12RootSignature* pSceneRoot;
        ID3D12PipelineState* pScenePSO;
        ID3D12DescriptorHeap* p_SC_RTVHeap;
        UINT mRTVDescriptorSize;
        UINT mCBV_SRV_UVADescriptorSize;

        ID3D12CommandAllocator* pFrameAllocator[3];
        ID3D12GraphicsCommandList8* pList;

        UINT mWidth;
        UINT mHeight;

        struct SceneConstantBuffer
        {
            DirectX::XMFLOAT4X4 mvp;
            DirectX::XMFLOAT4 light;
            float padding[44]; // Padding so the constant buffer is 256-byte aligned.
        };
        static_assert((sizeof(SceneConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");
        SceneConstantBuffer* sceneCB;
        UINT8* pCbvDataBegin;

        ID3D12Resource* pConstantBuffer;

        CD3DX12_CPU_DESCRIPTOR_HANDLE mTextureDescriptorHandle;

        ID3D12DescriptorHeap* pSceneHeapPool;

        Camera* pCamera;
    };

    Scene* CreateScene(SceneDesc* pDesc) {
        return new AScene(pDesc);
    }
    void RemoveScene(Scene* pScene) {
        auto temp_ptr = dynamic_cast<AScene*>(pScene);
        delete temp_ptr;
    }
}