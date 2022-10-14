add_executable(TheAftermath WIN32 
	"TheAftermath.cpp" 
	"GameScene.cpp" 
	"GameScene.h"
	"Device.cpp"
	"Device.h"
)
target_compile_definitions(TheAftermath PRIVATE UNICODE _UNICODE)
target_link_libraries(TheAftermath PRIVATE nvrhi nvrhi_d3d12 d3d12 dxgi dxguid)