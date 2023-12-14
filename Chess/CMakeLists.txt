project(Chess CXX)

add_executable(Chess
	"src/Core/Base.h"
	"src/Core/ChessApp.h"
	"src/Core/ChessApp.cpp"

	"src/Renderer/ColorTarget.h"
	"src/Renderer/DataBuffer.h"
	"src/Renderer/DataBuffer.cpp"
	"src/Renderer/Renderer.h"
	"src/Renderer/Renderer.cpp"
	"src/Renderer/RenderPipeline.h"
	"src/Renderer/RenderPipeline.cpp"
	"src/Renderer/RenderPipelineBuilder.h"
	"src/Renderer/RenderPipelineBuilder.cpp"
	"src/Renderer/Shader.h"
	"src/Renderer/Shader.cpp"
	"src/Renderer/Texture.h"
	"src/Renderer/Texture.cpp"
	"src/Renderer/VertexBuffer.h"
	"src/Renderer/VertexBuffer.cpp"
)

target_compile_definitions(Chess
	PRIVATE
	"_CRT_SECURE_NO_WARNINGS"

	PUBLIC
	$<$<CONFIG:Debug>:SS_DEBUG>
	$<$<CONFIG:Release>:SS_RELEASE>

	$<$<CONFIG:RelWithDebInfo>:SS_REL_DEB_INFO>
)

add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/imgui "vendor/imgui")
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/glm "vendor/glm")
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/tinyobjloader "vendor/tinyobjloader")

target_link_libraries(Chess PUBLIC IMGUI_LIB GLM_LIB TINY_OBJ_LOADER_LIB)

ConfigureProject()

if (EMSCRIPTEN)
	target_compile_definitions(Chess PRIVATE PLATFORM_WEB)
elseif(WIN32)
	target_compile_definitions(Chess PRIVATE PLATFORM_WINDOWS)
elseif(APPLE)
	target_compile_definitions(Chess PRIVATE PLATFORM_MAC)
elseif(UNIX)
	target_compile_definitions(Chess PRIVATE PLATFORM_LINUX)
endif()

set_property(TARGET Chess PROPERTY CXX_STANDARD 20)

target_include_directories(Chess PUBLIC ${WEBGPU_LIB_SOURCE_DIR}/src)
target_include_directories(Chess PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(Chess PUBLIC WEBGPU_LIB)
