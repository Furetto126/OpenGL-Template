#include "ComputeShader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>

#define OPENGL_MESSAGE_BUFFER_SIZE 512

ComputeShader::ComputeShader(const char* computeShaderCode, const char* vertexShaderCode, const char* fragmentShaderCode)
                : renderShader(std::make_shared<Shader>(vertexShaderCode, fragmentShaderCode))
{
        const uint32_t compute = glCreateShader(GL_COMPUTE_SHADER);;
        glShaderSource(compute, 1, &computeShaderCode, nullptr);
        glCompileShader(compute);

        int32_t success{};
        glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
        if (!success) {
                char infoLog[OPENGL_MESSAGE_BUFFER_SIZE];
                glGetShaderInfoLog(compute, OPENGL_MESSAGE_BUFFER_SIZE, nullptr, infoLog);
                std::cout << "Shader compilation failed! " << infoLog << std::endl;
        }

        programID = glCreateProgram();
        glAttachShader(programID, compute);
        glLinkProgram(programID);

        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        if (!success) {
                char infoLog[OPENGL_MESSAGE_BUFFER_SIZE];
                glGetShaderInfoLog(programID, OPENGL_MESSAGE_BUFFER_SIZE, nullptr, infoLog);
                std::cout << "Shader compilation failed! " << infoLog << std::endl;
        }
}

void ComputeShader::drawFullScreenQuad(const uint32_t width, const uint32_t height, const uint32_t binding)
{
        if (texWidth != width || texHeight != height)
                initDrawToTexture(width, height, binding);

        const uint32_t numGroupsX = (texWidth  + localSizeX - 1) / localSizeX;
        const uint32_t numGroupsY = (texHeight + localSizeY - 1) / localSizeY;

        use();
        glDispatchCompute(numGroupsX, numGroupsY, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        renderShader->drawFullScreenQuad();
}

void ComputeShader::initDrawToTexture(const uint32_t width, const uint32_t height, const uint32_t binding)
{
        if (texture != 0)
                glDeleteTextures(1, &texture);

        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glBindImageTexture(binding, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        texWidth = width;
        texHeight = height;
}

void ComputeShader::use() const { glUseProgram(programID); }
