#include "Scene.h"
#include "Camera.h"
#include "../Utility/Utility.h"
#include "../Utility/Json.h"

// 3rd
#include "d3dx12.h"
#include "json.hpp"

#include <combaseapi.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstdint>
#include <vector>
#include <DirectXMath.h>
namespace TheAftermath {

	struct SceneVertex {
        float Position[4];
        float Normal[3];
        float UV0[2];
        uint32_t ID;
    };

	struct GBufferCB
	{
		DirectX::XMFLOAT4X4 v;
		DirectX::XMFLOAT4X4 p;
		DirectX::XMFLOAT4 light;
		float padding[28];
	};
	static_assert((sizeof(GBufferCB) % 256) == 0, "Constant Buffer size must be 256-byte aligned");

	class AModernScene : public Scene {
	public:
		AModernScene(SceneDesc* pDesc) {
			pDevice = pDesc->pDevice;
			//
			TextureDesc texDesc;
			texDesc.pDevice = pDevice;
			texDesc.mFilePath = L"Asset/test.png";
			pTexture = CreateTexture(&texDesc);
			//GBuffer Pass
			auto GBufferVS = ReadData(L"GBufferVS.cso");
			auto GBufferPS = ReadData(L"GBufferPS.cso");
			pDevice->GetDevice()->CreateRootSignature(0, GBufferVS.data(), GBufferVS.size(), IID_PPV_ARGS(&pGBufferRoot));
			
            D3D12_INPUT_ELEMENT_DESC gubfferInputDesc[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "ID", 0, DXGI_FORMAT_R32_UINT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            };

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
			psoDesc.InputLayout = { gubfferInputDesc, 4 };
			psoDesc.pRootSignature = pGBufferRoot;
			psoDesc.VS = { GBufferVS.data(), GBufferVS.size() };
			psoDesc.PS = { GBufferPS.data(), GBufferPS.size() };
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.SampleMask = 0xffffffff;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
			psoDesc.SampleDesc.Count = 1;
			pDevice->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pGBufferPipeline));
			// Camera
			float w = pDevice->GetViewportWidth(), h = pDevice->GetViewportHeight();
			mCamera.SetPerspectiveMatrix(DirectX::XM_PIDIV4, w / h, 1.f, 1000.f);
			// Scene Descriptor Heap
			D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
			HeapDesc.NumDescriptors = 10000;
			HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			pDevice->GetDevice()->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&pSceneDescriptorHeap));
			// GBufferCB
			mSceneCV_CBV = pSceneDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			UINT BUFFERSIZE = sizeof(GBufferCB);
            auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(BUFFERSIZE);
            pDevice->GetDevice()->CreateCommittedResource(
                &heapProp,
                D3D12_HEAP_FLAG_NONE,
                &resDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&pSceneCB));
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
            cbvDesc.BufferLocation = pSceneCB->GetGPUVirtualAddress();
            cbvDesc.SizeInBytes = BUFFERSIZE;
            pDevice->GetDevice()->CreateConstantBufferView(&cbvDesc, mSceneCV_CBV);
			// MAP
			void* pCbvDataBegin;
			pSceneCB->Map(0, nullptr, reinterpret_cast<void**>(&pCbvDataBegin));
			pSceneCB_CPU = (GBufferCB*)pCbvDataBegin;
			// gbuffer
		
			GBufferDesc gbufferDesc;
			gbufferDesc.pDevice = pDevice;
			gbufferDesc.mWidth = pDevice->GetViewportWidth();
			gbufferDesc.mHeight = pDevice->GetViewportHeight();
			pGbuffer = CreateGBuffer(&gbufferDesc);
			// gbuffer Descriptor Handle
			mBaseHandle = mSceneCV_CBV;
			mCBV_SRV_UVADescriptorSize = pDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			mBaseHandle.Offset(1, mCBV_SRV_UVADescriptorSize);
			// gbuffer base color srv
			pDevice->GetDevice()->CreateShaderResourceView(pGbuffer->GetBaseColorResource(), nullptr, mBaseHandle);

			// Texture Descriptor Handle
			mTextureDescriptorHandle = mBaseHandle;
            mTextureDescriptorHandle.Offset(1, mCBV_SRV_UVADescriptorSize);

		}
		~AModernScene() {
			pSceneCB->Unmap(0, nullptr);
			pSceneCB->Release();
			pSceneDescriptorHeap->Release();
			pGBufferPipeline->Release();
			pGBufferRoot->Release();
			RemoveTexture(pTexture);
			RemoveGBuffer(pGbuffer);
		}

		void LoadStaticModel(const wchar_t* filePath) {
			std::filesystem::path modelParentPath{ filePath };
			std::filesystem::path modelStem = modelParentPath.stem();
			std::filesystem::path modelJsonPath = modelParentPath / modelStem; 
			modelJsonPath.concat(".json");
			std::filesystem::path modelBinPath = modelParentPath / modelStem;
			modelBinPath.concat(".bin");

		    std::ifstream modelJsonFstream(modelJsonPath);
			nlohmann::json modelData = nlohmann::json::parse(modelJsonFstream);

			const auto modelBinFileSize = std::filesystem::file_size(modelBinPath);
            std::ifstream modelBinFstream(modelBinPath);
            char* binStr = new char[modelBinFileSize + 1] {};
            modelBinFstream.read(binStr, modelBinFileSize);

			const auto meshCountStr = modelData["MeshCount"].get<std::string>();
			const auto meshCount = std::stoi(meshCountStr);

			auto modelMeshAttributes = modelData["MeshAttributes"];
			for (int meshIndex = 0; meshIndex < meshCount; ++meshIndex) {
				const auto meshData = modelMeshAttributes[meshIndex];

                const auto vertexOffsetStr = meshData["VertexOffset"].get<std::string>();
                const auto vertexOffset = std::stoi(vertexOffsetStr);
                const auto vertexCountStr = meshData["VertexCount"].get<std::string>();
                const auto vertexCount = std::stoi(vertexCountStr);

				constexpr int bakeModelVertexSize = sizeof(float) * 8;
				auto offset = vertexOffset;
				for (int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex) {
					SceneVertex vertex;
                    float* vertex_buffer = reinterpret_cast<float*>(&binStr[offset]);
					vertex.Position[0] = vertex_buffer[0];
					vertex.Position[1] = vertex_buffer[1];
					vertex.Position[2] = vertex_buffer[2];
					vertex.Position[3] = 1.f;
					vertex.Normal[0] = vertex_buffer[3];
					vertex.Normal[1] = vertex_buffer[4];
					vertex.Normal[2] = vertex_buffer[5];
					vertex.UV0[0] = vertex_buffer[6];
					vertex.UV0[1] = vertex_buffer[7];
					vertex.ID = mVertexID;
                    mSceneVertex.push_back(vertex);
                    offset += bakeModelVertexSize;
				}
				++mVertexID;

				const auto indexOffsetStr = meshData["IndexOffset"].get<std::string>();
                const auto indexOffset = std::stoi(indexOffsetStr);
                const auto indexCountStr = meshData["IndexCount"].get<std::string>();
                const auto indexCount = std::stoi(indexCountStr);
                const auto indexLastSize = mSceneIndex.size();
                uint32_t* index_buffer = reinterpret_cast<uint32_t*>(&binStr[offset]);
                for (int indexIndex = 0; indexIndex < indexCount; ++indexIndex) {
                    mSceneIndex.push_back(index_buffer[indexIndex] + indexLastSize);
                }


			}
			delete[]binStr;
		}

		void UpdateGbuffer() {
			auto native = mCamera.GetNative();
			pSceneCB_CPU->v = DirectX::XMFLOAT4X4(&native[0]);
			pSceneCB_CPU->p = DirectX::XMFLOAT4X4(&native[16]);
			pSceneCB_CPU->light = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);
		}

		void Update() {

			mCamera.Pitch( -0.1f);
			mCamera.UpdateViewMatrix();

			UpdateGbuffer();

			pDevice->BeginDraw();
			pDevice->DrawTexture(pTexture->GetSRV());
			pDevice->EndDraw();
			pDevice->Present();
		}

		UINT mCBV_SRV_UVADescriptorSize;

		GraphicsDevice* pDevice;
		GBuffer* pGbuffer;
		Texture* pTexture;
		//
		Camera mCamera;
		//
		ID3D12RootSignature* pGBufferRoot;
		ID3D12PipelineState* pGBufferPipeline;
		//
		
		//
		ID3D12DescriptorHeap* pSceneDescriptorHeap;

		D3D12_CPU_DESCRIPTOR_HANDLE mSceneCV_CBV;
		ID3D12Resource* pSceneCB;
		GBufferCB* pSceneCB_CPU;

		CD3DX12_CPU_DESCRIPTOR_HANDLE mBaseHandle;

		CD3DX12_CPU_DESCRIPTOR_HANDLE mTextureDescriptorHandle;
		//
		
		//
		uint32_t mVertexID = 0;
		std::vector<SceneVertex> mSceneVertex;
		std::vector<uint32_t> mSceneIndex;
	};

	Scene* CreateScene(SceneDesc* pDesc) {
       return new AModernScene(pDesc);
    }
    void RemoveScene(Scene* pScene) {
		auto temp_ptr = dynamic_cast<AModernScene*>(pScene);
		delete temp_ptr;
    }
}