#include "Shader.h"
#include <Windows.h>
#include <d3dcompiler.h>
#include <filesystem>

namespace TheAftermath {

	std::vector<uint8_t> LoadCSO(const wchar_t* filePath) {
		ID3DBlob* pBlob;
		MAX_PATH;

		if (auto hr = D3DReadFileToBlob(filePath, &pBlob);hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			wchar_t moduleName[MAX_PATH] = {};
			GetModuleFileNameW(nullptr, moduleName, MAX_PATH);
			std::filesystem::path parentPath{ moduleName };
			parentPath = parentPath.parent_path();

			std::filesystem::path relativeFilePath{ filePath };
			relativeFilePath = relativeFilePath.filename();
			auto myPath = parentPath / relativeFilePath;
			auto myPathStr = myPath.wstring();

			D3DReadFileToBlob(myPathStr.c_str(), &pBlob);
		}
		std::vector<uint8_t> ret;
		for (SIZE_T i = 0;i < pBlob->GetBufferSize(); ++i) {
			uint8_t* ptr = (uint8_t*)pBlob->GetBufferPointer();
			ret.push_back(ptr[i]);
		}

		pBlob->Release();
		return ret;
	}

//    std::vector<uint8_t> ReadData(_In_z_ const wchar_t* name)
//    {
//        std::ifstream inFile(name, std::ios::in | std::ios::binary | std::ios::ate);
//
//#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
//        if (!inFile)
//        {
//            wchar_t moduleName[_MAX_PATH] = {};
//            if (!GetModuleFileNameW(nullptr, moduleName, _MAX_PATH))
//                throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "GetModuleFileNameW");
//
//            wchar_t drive[_MAX_DRIVE];
//            wchar_t path[_MAX_PATH];
//
//            if (_wsplitpath_s(moduleName, drive, _MAX_DRIVE, path, _MAX_PATH, nullptr, 0, nullptr, 0))
//                throw std::runtime_error("_wsplitpath_s");
//
//            wchar_t filename[_MAX_PATH];
//            if (_wmakepath_s(filename, _MAX_PATH, drive, path, name, nullptr))
//                throw std::runtime_error("_wmakepath_s");
//
//            inFile.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
//        }
//#endif
//
//        if (!inFile)
//            throw std::runtime_error("ReadData");
//
//        const std::streampos len = inFile.tellg();
//        if (!inFile)
//            throw std::runtime_error("ReadData");
//
//        std::vector<uint8_t> blob;
//        blob.resize(size_t(len));
//
//        inFile.seekg(0, std::ios::beg);
//        if (!inFile)
//            throw std::runtime_error("ReadData");
//
//        inFile.read(reinterpret_cast<char*>(blob.data()), len);
//        if (!inFile)
//            throw std::runtime_error("ReadData");
//
//        inFile.close();
//
//        return blob;
//    }
}
