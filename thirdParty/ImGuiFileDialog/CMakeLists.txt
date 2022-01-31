set(ImGUIFileDialog_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ImGuiFileDialog")

message(STATUS "IMGUI_INCLUDE_DIRS ${IMGUI_INCLUDE_DIRS}")

buildAndPackageLib("ImGuiFileDialog"
	COMPONENT "ImGuiFileDialog"
	DESCRIPTION "ImGuiFileDialog from https://github.com/aiekick/ImGuiFileDialog/tree/Lib_Only before I package it"
	PUBLIC_INCLUDES ${ImGUIFileDialog_DIR}
	PRIVATE_INCLUDES ${IMGUI_INCLUDE_DIRS}
	PRIVATE_LIBS ${IMGUI_LIBRARIES} glfw GLEW::GLEW OpenGL::GLU
)
