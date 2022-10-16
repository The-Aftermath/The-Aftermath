add_executable(BakeModel "BakeModel.cpp")

target_link_libraries(BakeModel PRIVATE assimp)
add_custom_command (TARGET BakeModel POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:assimp> $<TARGET_FILE_DIR:BakeModel>
)
