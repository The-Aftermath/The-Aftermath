add_executable(TheAftermath WIN32 "TheAftermath.cpp" "GameContext.cpp" "GameContext.h")
target_compile_definitions(TheAftermath PRIVATE UNICODE _UNICODE)
target_link_libraries(TheAftermath PRIVATE nvrhi nvrhi_d3d12)