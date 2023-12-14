#include "WebgpuLib.h"

#include <imgui/imgui.h>

#include <tinyobjloader/tiny_obj_loader.h>

class PlaygroundApp : public Base::Application
{
public:
	PlaygroundApp()
	{
		tinyobj::attrib_t attributes;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warning, error;
		tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, "C:/Users/Martin/Desktop/Chess.obj", "C:/Users/Martin/Desktop/");
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
