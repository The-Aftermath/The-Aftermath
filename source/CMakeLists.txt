add_executable(game WIN32 
	"TheAftermath.cpp" 
	"GameScene.cpp" 
	"GameScene.h"
	"Device.cpp"
	"Device.h"
	"Mesh.h"
)

target_compile_options(game PRIVATE /W3)
target_compile_definitions(game PRIVATE UNICODE _UNICODE NOMINMAX)
target_link_libraries(game PRIVATE nvrhi nvrhi_d3d12 d3d12.lib dxgi.lib dxguid.lib)
target_include_directories(game PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/cgltf")

