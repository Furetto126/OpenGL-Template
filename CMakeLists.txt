cmake_minimum_required(VERSION 3.28)

# Set project name and version
project(OpenGL-Template LANGUAGES C CXX)

# Set C/C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# IMPORTANT!
option(USE_WIN32 "Use the Win32 OpenGL implementation" OFF)
message(STATUS "USE_WIN32 is set to: ${USE_WIN32}")

# Recursively find all source files in source directory
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCE_FILES
        ${SRC_DIR}/*.cpp
        ${SRC_DIR}/*.c
)
# Remove files for glfw or win32 Apps, we will specify which of the two to build later.
list(FILTER SOURCE_FILES EXCLUDE REGEX ".*/AppGL/OpenGL/(glfw|win32)/.*")

if(USE_WIN32)
    file(GLOB_RECURSE PLATFORM_SOURCES
            ${SRC_DIR}/AppGL/OpenGL/win32/*.cpp
            ${SRC_DIR}/AppGL/OpenGL/win32/*.c
    )
else()
    file(GLOB_RECURSE PLATFORM_SOURCES
            ${SRC_DIR}/AppGL/OpenGL/glfw/*.cpp
            ${SRC_DIR}/AppGL/OpenGL/glfw/*.c
    )
endif()

# Combine the common sources (with platform-specific files filtered out) with the chosen platform sources.
list(APPEND SOURCE_FILES ${PLATFORM_SOURCES})
add_executable(OpenGL-Template ${SOURCE_FILES})

# Include libraries
set(LIB_DIR ${PROJECT_SOURCE_DIR}/libs)

# OpenGL (GLAD)
# -------------
set(GLAD_DIR     ${LIB_DIR}/glad)
set(GLAD_INCLUDE ${GLAD_DIR}/include)

target_sources(OpenGL-Template PRIVATE "${GLAD_DIR}/src/glad.c")
target_include_directories(OpenGL-Template PRIVATE ${GLAD_INCLUDE})

    # WGL (Win32)
    if(USE_WIN32)
        target_link_libraries(OpenGL-Template PRIVATE opengl32)
    endif()

# GLFW
# ----
if(NOT(USE_WIN32))
    set(GLFW_DIR     ${LIB_DIR}/glfw)
    set(GLFW_INCLUDE ${GLFW_DIR}/include)

    add_subdirectory(${GLFW_DIR})

    target_include_directories(OpenGL-Template PRIVATE ${GLFW_INCLUDE})
    target_link_libraries(OpenGL-Template PRIVATE glfw)
    target_compile_definitions(OpenGL-Template PRIVATE GLFW_STATIC)
endif()

# GLM
# ---
set(GLM ${LIB_DIR}/glm)
target_include_directories(OpenGL-Template PRIVATE ${GLM})