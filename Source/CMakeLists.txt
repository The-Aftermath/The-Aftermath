add_executable(TheAftermathProject WIN32 
	"Main.cpp"
	"Graphics.cpp"
	"Graphics.h"
	"Scene.h"
	"Scene.cpp"
	"AObject.h"
	"AObject.cpp"
	"Utility.h"

	"OutputVS.hlsl"
	"OutputPS.hlsl"
	"OutputRS.hlsl"
)
target_link_libraries(TheAftermathProject PRIVATE d3d12.lib dxgi.lib dxguid.lib)
target_compile_definitions(TheAftermathProject PRIVATE UNICODE)