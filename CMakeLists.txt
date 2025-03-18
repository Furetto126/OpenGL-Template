cmake_minimum_required(VERSION 3.28)

# Set project name and version
project(OpenGL_Template_App LANGUAGES C CXX)

# Set C/C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Recursively find all source files in source directory
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCE_FILES
        ${SRC_DIR}/*.cpp
        ${SRC_DIR}/*.c
)

add_executable(OpenGL_Template_App ${SOURCE_FILES})

# Include libraries
set(LIB_DIR ${PROJECT_SOURCE_DIR}/libs)

# OpenGL (GLAD)
# -------------
set(GLAD_DIR     ${LIB_DIR}/glad)
set(GLAD_INCLUDE ${GLAD_DIR}/include)

target_sources(OpenGL_Template_App PRIVATE "${GLAD_DIR}/src/glad.c")
target_include_directories(OpenGL_Template_App PRIVATE ${GLAD_INCLUDE})

# GLFW
# ----
set(GLFW_DIR     ${LIB_DIR}/glfw)
set(GLFW_INCLUDE ${GLFW_DIR}/include)

add_subdirectory(${GLFW_DIR})

target_include_directories(OpenGL_Template_App PRIVATE ${GLFW_INCLUDE})
target_link_libraries(OpenGL_Template_App PRIVATE glfw)
target_compile_definitions(OpenGL_Template_App PRIVATE GLFW_STATIC)

# GLM
# ---
set(GLM ${LIB_DIR}/glm)
target_include_directories(OpenGL_Template_App PRIVATE ${GLM})

# GSPROP
# ------
set(GSPROP ${LIB_DIR}/gsprop)
target_include_directories(OpenGL_Template_App PRIVATE ${GSPROP})