#pragma once
#include "bgfx/c99/bgfx.h"
#include <filesystem>
#include <fstream>
#include <cstdint>
namespace TheAftermath {
	
	inline bgfx_program_handle_t loadProgram(const char* _vsName, const char* _fsName) {
		std::filesystem::path vsRPath{ _vsName }, fsRPath{ _fsName };
		vsRPath += ".bin"; fsRPath += ".bin";
		std::filesystem::path parentPath{ "ShaderBin" };
		auto vsPath = parentPath / vsRPath;
		auto fsPath = parentPath / fsRPath;

		auto vsSize = std::filesystem::file_size(vsPath);
		const bgfx_memory_t *vsMem = bgfx_alloc(vsSize + 1);
		auto fsSize = std::filesystem::file_size(fsPath);
		const bgfx_memory_t* fsMem = bgfx_alloc(fsSize + 1);
		
		uint8_t* vsFileData = new uint8_t[vsSize + 1]{};
		uint8_t* fsFileData = new uint8_t[fsSize + 1]{};

		std::ifstream vsStream(vsPath);
		std::ifstream fsStream(fsPath);

		vsStream.read((char*)vsFileData, vsSize);
		fsStream.read((char*)fsFileData, fsSize);

		for (uintmax_t i = 0;i < vsSize; ++i) {
			vsMem->data[i] = vsFileData[i];
		}
		for (uintmax_t i = 0; i < fsSize; ++i) {
			fsMem->data[i] = fsFileData[i];
		}

		delete[]vsFileData;
		delete[]fsFileData;
		vsMem->data[vsSize] = '\0';
		fsMem->data[fsSize] = '\0';

		auto vsHandle = bgfx_create_shader(vsMem);
		auto fsHandle = bgfx_create_shader(fsMem);

		bgfx_set_shader_name(vsHandle, _vsName, INT32_MAX);
		bgfx_set_shader_name(fsHandle, _fsName, INT32_MAX);
		return bgfx_create_program(vsHandle, fsHandle, true);
	}
}
