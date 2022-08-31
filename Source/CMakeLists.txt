add_executable(TheAftermathProject WIN32 "Main.cpp")
target_link_libraries(TheAftermathProject PRIVATE d3d12.lib dxgi.lib dxguid.lib)