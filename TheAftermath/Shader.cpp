#include "Shader.h"
#include <d3dcompiler.h>
namespace TheAftermath {
	std::vector<uint8_t> LoadCSO(const wchar_t* filePath) {
		ID3DBlob* pBlob;
		D3DReadFileToBlob(filePath, &pBlob);
		std::vector<uint8_t> ret;
		for (SIZE_T i = 0;i < pBlob->GetBufferSize(); ++i) {
			uint8_t* ptr = (uint8_t*)pBlob->GetBufferPointer();
			ret.push_back(ptr[i]);
		}

		pBlob->Release();
		return ret;
	}
}
