cmake_minimum_required(VERSION 3.26)

project (BeforeEngine CXX)

add_executable(before_main ${CMAKE_SOURCE_DIR}/source/main.cpp)

if (WIN32)
	add_compile_definitions(RHI_DX12)
	# use WINDOWS build in windows platform
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

target_include_directories(before_main PRIVATE ${CMAKE_SOURCE_DIR}/Source)

target_sources(before_main PRIVATE
	${CMAKE_SOURCE_DIR}/Source/DX12App.cpp
	${CMAKE_SOURCE_DIR}/Source/DxException.cpp
)

