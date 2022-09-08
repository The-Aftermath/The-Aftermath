#pragma once
#include "AObject.h"
#include "Device.h"
#include <d3d12.h>
namespace TheAftermath {
	struct CommandBufferDesc {
		Device* pDevice = nullptr;
		D3D12_COMMAND_LIST_TYPE mType = D3D12_COMMAND_LIST_TYPE_DIRECT;
	};

	struct CommandBuffer : public AObject {
		virtual ID3D12GraphicsCommandList4* GetCmdList() const = 0;
		virtual void Reset() = 0;
		virtual void Close() = 0;
	};
	CommandBuffer* CreateCommandBuffer(CommandBufferDesc* pDesc);
}
