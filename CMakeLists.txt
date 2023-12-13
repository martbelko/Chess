cmake_minimum_required(VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
	cmake_policy(SET CMP0141 NEW)
	set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if(CMAKE_VERSION VERSION_LESS_EQUAL 3.12)
	message(FATAL_ERROR "The projet needs C++20 version")
endif()

set_property(GLOBAL PROPERTY ASSETS_PATH_PROP "${CMAKE_SOURCE_DIR}/Chess/assets")

add_subdirectory(WebgpuLib)
add_subdirectory(Chess)
