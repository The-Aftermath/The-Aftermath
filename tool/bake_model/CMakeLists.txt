add_executable(BakeModel "BakeModel.cpp")

target_link_libraries(BakeModel PRIVATE assimp)
add_custom_command (TARGET BakeModel POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:assimp> $<TARGET_FILE_DIR:BakeModel>
)
target_compile_options(BakeModel PRIVATE /W3)
target_include_directories(BakeModel PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/cgltf")
target_include_directories(BakeModel PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/json")
target_include_directories(BakeModel PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/stb")
