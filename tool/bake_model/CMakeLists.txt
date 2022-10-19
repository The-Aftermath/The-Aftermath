add_executable(bake_model "BakeModel.cpp")

target_link_libraries(bake_model PRIVATE assimp WindowsApp.lib)
add_custom_command (TARGET bake_model POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:assimp> $<TARGET_FILE_DIR:bake_model>
)
target_compile_options(bake_model PRIVATE /W3)
target_include_directories(bake_model PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/cgltf")
target_include_directories(bake_model PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/json")
target_include_directories(bake_model PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/stb")