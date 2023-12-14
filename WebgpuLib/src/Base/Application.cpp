#include "Application.h"

#include "Base/Base.h"
#include "Base/Filesystem.h"
#include "Base/Utils.h"
#include "Base/Timer.h"

#include "Base/GraphicsContext.h"

#include <webgpu/webgpu_cpp.h>

#if defined(PLATFORM_WEB)
	#include <emscripten.h>
	#include <emscripten/html5.h>
	#include <emscripten/html5_webgpu.h>
#endif

namespace Base {

	Application::Application()
	{
		Log::Init();
		Filesystem::Init();

		WindowProps props{ .width = 1280, .height = 720, .title = "SVGscape" };
		m_Window = new Window(props);
	}

	Application::~Application()
	{
		delete m_Window;
	}

	void Application::Run()
	{
		m_Running = true;
#if defined(PLATFORM_WEB)
		emscripten_request_animation_frame_loop(Application::EMSRedraw, (void*)this);
#else
		Timer timer;
		while (m_Running)
		{
			Timestep ts = Timestep(timer.ElapsedNs());
			timer.Reset();

			OnFrame(ts);
		}
#endif
	}

	void Application::OnFrame(Timestep ts)
	{
		const WindowResizedEvent* lastWindowResizeEvent = nullptr;
		for (const Event& ev : m_Window->GetEvents())
		{
			switch (ev.type)
			{
			case EventType::WindowResized:
				lastWindowResizeEvent = &ev.as.windowResizedEvent;
				break;
			case EventType::WindowClosed:
				m_Running = false;
				return;
			}
		}

		Update(ts);

		if (m_Window->GetWidth() != 0 && m_Window->GetHeight() != 0)
		{
			Render();
		}

		m_Window->Update();
	}

#if defined(PLATFORM_WEB)
	EM_BOOL Application::EMSRedraw(double time, void* userData)
	{
		if (userData == nullptr)
		{
			return false;
		}

		Application* app = static_cast<Application*>(userData);
		app->OnFrame(Timestep(time));
		return true;
	}
#endif

}
