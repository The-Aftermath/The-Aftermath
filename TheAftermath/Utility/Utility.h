#pragma once
#include <Windows.h>
#include <sal.h>
#include <functional>
#include <utility>
#include <vector>
#include <cstdint>
namespace TheAftermath {
	template<typename Function,typename... Args>
    inline void RunLoop(Function&& f, Args&&... args) {
        MSG msg = {};
        while (WM_QUIT != msg.message)
        {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                std::invoke(std::forward<Function>(f), std::forward<Args>(args)...);
            }
        }
	}

    std::vector<uint8_t> ReadData(_In_z_ const wchar_t* name);
}
