#include "WebgpuLib.h"

#include <imgui/imgui.h>

class PlaygroundApp : public Base::Application
{
public:
	PlaygroundApp()
	{
	}

	virtual void Update(Base::Timestep ts) override
	{
		m_LastFrameTime = ts.GetSeconds();
	}

	virtual void Render() override
	{
	}

	virtual void RenderImGui() override
	{
	}
private:
	float m_LastFrameTime;
};

extern Base::Application* CreateApplication()
{
	return new PlaygroundApp();
}
