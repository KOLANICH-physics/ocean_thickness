cmake_minimum_required(VERSION 3.15)

project(Ocean_thickness)
set("PROJECT_DESCRIPTION" "A tool to measure thickness of absorbing films")
set("PROJECT_HOMEPAGE_URL" "https://github.com/KOLANICH-physics/ocean_thickness")
set(PROJECT_VERSION "0.1")

set(CPACK_PACKAGE_VENDOR "KOLANICH")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

set(OUR_CMAKE_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${OUR_CMAKE_MODULES_DIR}" "${OUR_CMAKE_MODULES_DIR}/Boilerplate" "${OUR_CMAKE_MODULES_DIR}/DoxygenUtils" "${OUR_CMAKE_3PARTY_MODULES_DIR}")

include(Boilerplate)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)

find_package(SeaBreeze REQUIRED)

pkg_check_modules(IMGUI REQUIRED "imgui")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/implot")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/tdigest")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/p2")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/ddsketch")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/ImGuiFileDialog")

find_package(DataSketches REQUIRED)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory("${SRC_DIR}")
