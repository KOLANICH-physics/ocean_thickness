// Based on: https://blog.conan.io/2019/06/26/An-introduction-to-the-Dear-ImGui-library.html

#include <iostream>
#include <vector>
#include <algorithm>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <implot.h>

struct GUI{
	GLFWwindow* window;

	GUI();
	virtual ~GUI();

	virtual void checkSettings() = 0;

	virtual void drawSettingsWindow() = 0;

	virtual void drawSpectrumWindow() = 0;

	void drawWindows();


	void initializeFrame();

	void finalizeFrame();

	virtual void processAcquiredData() = 0;

	void mainLoop();
};
