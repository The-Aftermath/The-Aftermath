set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/Shader/OutputVS.hlsl" PROPERTIES VS_SHADER_TYPE Vertex VS_SHADER_MODEL 6.6)
set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/Shader/OutputPS.hlsl" PROPERTIES VS_SHADER_TYPE Pixel VS_SHADER_MODEL 6.6)
add_executable(TheAftermathProject WIN32 
	"Main.cpp"
	"Graphics.cpp"
	"Graphics.h"
	"Scene.h"
	"Scene.cpp"
	"AObject.h"
	"AObject.cpp"
	"Utility.h"
	"Utility.cpp"

	"Shader/OutputVS.hlsl"
	"Shader/OutputPS.hlsl"
)
target_link_libraries(TheAftermathProject PRIVATE d3d12.lib dxgi.lib dxguid.lib)
target_compile_definitions(TheAftermathProject PRIVATE UNICODE)