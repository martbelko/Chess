#include "Base/Application.h"

#include <GLFW/glfw3.h>

#ifdef PLATFORM_WEB
	#include <emscripten.h>
	#include <emscripten/html5.h>
	#include <emscripten/html5_webgpu.h>
#endif

#include <webgpu/webgpu_cpp.h>

extern Base::Application* CreateApplication();

int main(int argc, char* argv[])
{
	Base::Application* app = CreateApplication();
	app->Run();
#if !defined(PLATFORM_WEB)
	delete app;
#endif
	return 0;
}
