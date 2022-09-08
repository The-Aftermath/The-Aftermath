#include "CommandBuffer.h"
#include <combaseapi.h>
namespace TheAftermath {

	struct ACommandBuffer : public CommandBuffer {

		ACommandBuffer(CommandBufferDesc* pDesc) {
			auto device = pDesc->pDevice->GetDevice();
			device->CreateCommandAllocator(pDesc->mType, IID_PPV_ARGS(&pAllocator));
			device->CreateCommandList1(0, pDesc->mType, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&pList));
		}
		~ACommandBuffer() {
			pAllocator->Release();
			pList->Release();
		}

		void Release() {
			delete this;
		}

		ID3D12GraphicsCommandList4* GetCmdList() const {
			return pList;
		}

		void Reset() {
			pAllocator->Reset();
			pList->Reset(pAllocator, nullptr);
		}

		void Close() {
			pList->Close();
		}

		ID3D12CommandAllocator* pAllocator;
		ID3D12GraphicsCommandList4* pList;
	};
	CommandBuffer* CreateCommandBuffer(CommandBufferDesc* pDesc) {
		return new ACommandBuffer(pDesc);
	}
}
