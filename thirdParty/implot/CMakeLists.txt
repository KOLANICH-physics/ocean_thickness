set(IMPLOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/implot")

message(STATUS "IMGUI_INCLUDE_DIRS ${IMGUI_INCLUDE_DIRS}")

buildAndPackageLib("implot"
	COMPONENT "implot"
	DESCRIPTION "implot from https://github.com/epezent/implot before I package it"
	PUBLIC_INCLUDES ${IMPLOT_DIR}
	PRIVATE_INCLUDES ${IMGUI_INCLUDE_DIRS}
	PRIVATE_LIBS ${IMGUI_LIBRARIES} glfw GLEW::GLEW OpenGL::GLU
)
