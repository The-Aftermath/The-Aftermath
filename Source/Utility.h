#pragma once
#include <sal.h>
#include <vector>
#include <cstdint>
namespace TheAftermath {
    std::vector<uint8_t> ReadData(_In_z_ const wchar_t* name);
}