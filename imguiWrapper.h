#ifndef IMGUIWRAPPER_H
#define IMGUIWRAPPER_H


#include "opengl_files/ImGui/imgui.h"
#include "opengl_files/ImGui/imgui_impl_glfw.h"
#include "opengl_files/ImGui/imgui_impl_opengl3.h"

class ImguiWrapper
{
public:
	//deleting copy constructor so it cant be copied
	ImguiWrapper(const ImguiWrapper&) = delete;

	static void init(GLFWwindow* window) { Get().Iinit(window); }
	static void frameStart() { Get().IframeStart(); }
	static void frameEnd() { Get().IframeEnd(); }
	static void terminate() { Get().Iterminate(); }

private:
	static ImguiWrapper& Get()
	{
		//this will be instanciated for the first use after that it will be recalled
		static ImguiWrapper instance;
		return instance;
	}

	void Iinit(GLFWwindow* window)
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 130");//glsl version
	}
	void IframeStart()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
	void IframeEnd()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	void Iterminate()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	//private constructor so it cant be instanciated outside
	ImguiWrapper() {}
};


#endif // !IMGUIWRAPPER_H
