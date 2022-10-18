add_executable(Game WIN32 
	"TheAftermath.cpp" 
	"GameScene.cpp" 
	"GameScene.h"
	"Device.cpp"
	"Device.h"
	"Mesh.h"
)

target_compile_options(Game PRIVATE /W3)
target_compile_definitions(Game PRIVATE UNICODE _UNICODE NOMINMAX)
target_link_libraries(Game PRIVATE nvrhi nvrhi_d3d12 d3d12 dxgi dxguid)
target_include_directories(Game PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/cgltf")

