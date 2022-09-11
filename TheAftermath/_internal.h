#pragma once
#include "bgfx/c99/bgfx.h"
#include <filesystem>
namespace TheAftermath {
	
	inline bgfx_program_handle_t loadProgram(const char* _vsName, const char* _fsName) {
		std::filesystem::path vsRPath{ _vsName }, fsRPath{ _fsName };
		vsRPath += ".bin"; fsRPath += ".bin";
		std::filesystem::path parentPath{ "ShaderBin" };
		auto vsPath = parentPath / vsRPath;
		auto fsPath = parentPath / fsRPath;
	}
}
