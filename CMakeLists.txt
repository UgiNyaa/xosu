cmake_minimum_required(VERSION 3.5)
set(CMAKE_BUILD_TYPE Debug)

project(osu)

file(GLOB_RECURSE osu_SOURCES "src/*.c")
file(GLOB_RECURSE osu_HEADERS "src/*.h")

file(COPY resources DESTINATION .)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GLEW_LIBRARIES})

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GLFW_LIBRARIES})

add_executable(osu ${osu_SOURCES} ${osu_HEADERS})
target_link_libraries(osu ${LIBS})