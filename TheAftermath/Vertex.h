#pragma once
#include <cstdint>
namespace TheAftermath {
	struct Vertex {
		float Position[4];
		float Normal[3];
		float UV0[3];
		uint32_t ID;
	};
}
