cmake_minimum_required(VERSION 3.13)

project("The Aftermath")

if(MSVC)
	set(CMAKE_CXX_STANDARD 20)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS ON)

	set(NVRHI_WITH_VULKAN OFF CACHE BOOL "" FORCE)
	set(NVRHI_WITH_SHADER_COMPILER OFF CACHE BOOL "" FORCE)

	add_subdirectory("3rdparty/nvrhi")
	add_subdirectory("3rdparty/assimp")
	add_subdirectory("tool/bake_model")
	add_subdirectory("source")
else()
	message(FATAL_ERROR "Please choose MSVC.")
endif()