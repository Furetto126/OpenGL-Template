#pragma once

#include "glad/glad.h"

namespace AppGL
{
        void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                const GLchar* message, const void* userParam);
}