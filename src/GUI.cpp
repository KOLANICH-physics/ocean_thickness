// Based on: https://blog.conan.io/2019/06/26/An-introduction-to-the-Dear-ImGui-library.html

#include "GUI.hpp"

GUI::GUI(){
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1280, 720, "Ocean Thickness Meter", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glewInit();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImPlot::CreateContext();
}

GUI::~GUI(){
	ImPlot::DestroyContext();
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}

void GUI::drawWindows(){
	drawSettingsWindow();
	drawSpectrumWindow();
}


void GUI::initializeFrame(){
	glfwPollEvents();
	glClearColor(0.0f, 0.0f, 0.0f, 1.00f);
	glClear(GL_COLOR_BUFFER_BIT);

	// feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GUI::finalizeFrame(){
	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glfwSwapBuffers(window);
}

void GUI::mainLoop(){
	while (!glfwWindowShouldClose(window)) {
		checkSettings();
		processAcquiredData();
		initializeFrame();
		drawWindows();
		finalizeFrame();
	}
}
