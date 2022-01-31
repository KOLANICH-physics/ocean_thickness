file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(ocean_thickness ${SRCS})
harden(ocean_thickness)

target_include_directories(ocean_thickness PRIVATE ${IMGUI_INCLUDE_DIRS} ${DATASKETCHES_INCLUDE_DIR} ${INCLUDES_DIR})
target_link_libraries(ocean_thickness PRIVATE ${IMGUI_LIBRARIES} glfw GLEW::GLEW OpenGL::GLU ImGuiFileDialog implot tdigest p2 ddsketch SeaBreeze ${DATASKETCHES_LIB})
