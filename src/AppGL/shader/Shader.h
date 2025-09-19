#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <string_view>
#include <cstdint>
#include <any>
#include <algorithm>
#include <string>

#include "SSBO.h"
#include "../base.h"

class Shader {
public:
        Shader(const std::string_view &vertexCode, const std::string_view &fragmentCode);

        void use () const;
        void drawFullScreenQuad() const;

        template<typename T>
        void addSSBO(SSBO<T>& ssbo);
        template<typename T>
        void setSSBO(SSBO<T>& ssbo);

        template<typename T>
        void addUniform(const std::string &name, const T &value);
        template<typename T>
        void setUniform(const std::string &name, const T &value);

private:
        uint32_t m_VAO        = 0;
        uint32_t m_VBO        = 0;
        uint32_t m_EBO        = 0;
        uint32_t m_vertexID   = 0;
        uint32_t m_fragmentID = 0;
        uint32_t m_programID  = 0;

        std::vector<std::string> m_initializedUniforms{};

        uint32_t m_nextFreeSSBOBinding{};
        std::vector<SSBO<std::any>> m_SSBOs{};

        static void compileShader(const char* code, GLenum type, uint32_t &id);
        void linkShaders();
        void initFullScreenQuadVertices();
};

template<typename T>
void Shader::addUniform(const std::string &name, const T &value)
{
        bool isPresent = std::any_of(m_initializedUniforms.begin(), m_initializedUniforms.end(), [name](const std::string &u) -> bool {
                return u == name;
        });

        if (isPresent)
                std::cout << "Cannot add uniform as it was not already initialized. Name: " << name << std::endl;

        m_initializedUniforms.push_back(name);

        this->use();
        int uniformLocation = glGetUniformLocation(m_programID, name.data());

        CONSTEXPR_SWITCH(T,
                CASE(bool,      glUniform1i(uniformLocation, value);                                     return; ),
                CASE(int,       glUniform1i(uniformLocation, value);                                     return; ),
                CASE(float,     glUniform1f(uniformLocation, value);                                     return; ),
                CASE(glm::vec2, glUniform2f(uniformLocation, value.x, value.y);                          return; ),
                CASE(glm::vec3, glUniform3f(uniformLocation, value.x, value.y, value.z);                 return; ),
                CASE(glm::vec4, glUniform4f(uniformLocation, value.x, value.y, value.z, value.w);        return; ),
                CASE(glm::mat4, glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value)); return; ),
        );
        std::cout << "Type not supported for uniforms" << std::endl;
}

template<typename T>
void Shader::setUniform(const std::string &name, const T &value)
{
        bool isPresent = std::any_of(m_initializedUniforms.begin(), m_initializedUniforms.end(), [name](const std::string &u) -> bool {
                return u == name;
        });

        if (!isPresent)
                std::cout << "Cannot set uniform as it was not already initialized. Name: " << name << std::endl;

        this->use();
        const int location = glGetUniformLocation(m_programID, name.data());

        CONSTEXPR_SWITCH(T,
                CASE(bool,      glUniform1i(location, value);                                     return; ),
                CASE(int,       glUniform1i(location, value);                                     return; ),
                CASE(float,     glUniform1f(location, value);                                     return; ),
                CASE(glm::vec2, glUniform2f(location, value.x, value.y);                          return; ),
                CASE(glm::vec3, glUniform3f(location, value.x, value.y, value.z);                 return; ),
                CASE(glm::vec4, glUniform4f(location, value.x, value.y, value.z, value.w);        return; ),
                CASE(glm::mat4, glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value)); return; ),
        );
        std::cout << "Type not supported for uniforms" << std::endl;
}

template<typename T>
void Shader::addSSBO(SSBO<T> &ssbo)
{
        if (ssbo.binding != m_nextFreeSSBOBinding)
                std::cout << "Cannot initialize SSBO in a non-increasing order.\nBinding '" << ssbo.binding <<
                        "' with next free binding '" << m_nextFreeSSBOBinding << "'" << std::endl;

        if (!ssbo.pData || ssbo.pData->empty())
                std::cout << "Cannot modify SSBO as it is empty. Binding '" << ssbo.binding << "'" << std::endl;

        // Bind the SSBO
        glGenBuffers(1, &ssbo.bufferID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo.bufferID);

        // If the SSBO should include its size as an int then it will be embedded with some padding at the start
        size_t offset = ssbo.hasSizeAsUniform ? SSBO<T>::alignment : 0;
        size_t totalSize = ssbo.pData->size() * sizeof(T) + offset;
        void *data = std::malloc(totalSize);

        if (!data) {
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
                std::cout << "Failed to allocate memory for SSBO." << std::endl;
        }

        // Copy the contents into the memory block
        if (ssbo.hasSizeAsUniform)
                *(int *)data = static_cast<int>(ssbo.pData->size());

        std::memcpy(static_cast<char*>(data) + offset, ssbo.pData->data(), ssbo.pData->size() * sizeof(T));

        // Send the SSBO to the GPU
        glBufferData(GL_SHADER_STORAGE_BUFFER, (uint32_t)totalSize, data, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo.binding, ssbo.bufferID);

        free(data);

        // Unbind and increment counter
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        m_nextFreeSSBOBinding++;
}

template<typename T>
void Shader::setSSBO(SSBO<T> &ssbo)
{
        if (ssbo.binding >= m_nextFreeSSBOBinding)
                std::cout << "Cannot modify a non-initialized SSBO at binding '" << ssbo.binding << "'" << std::endl;

        ssbo.needsUpdate = false;
        if (!ssbo.pData || ssbo.pData->empty())
                std::cout << "Cannot modify SSBO as it is empty. Binding '" << ssbo.binding << "'" << std::endl;

        // Bind SSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo.bufferID);

        // Get the size of the already present SSBO in the GPU
        int oldBufferSize{};
        glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_SIZE, &oldBufferSize);

        size_t offset = ssbo.hasSizeAsUniform ? SSBO<T>::alignment : 0;

        // If size is different than previous
        if (offset + ssbo.pData->size() * sizeof(T) != oldBufferSize) {
                auto totalSize = (uint32_t)offset + ssbo.pData->size() * sizeof(T);
                void* data = std::malloc(totalSize);

                if (!data) {
                        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
                        std::cout << "Failed to allocate memory for SSBO." << std::endl;
                }

                if (ssbo.hasSizeAsUniform) {
                        int vectorSize = static_cast<int>(ssbo.pData->size());
                        std::memcpy(data, &vectorSize, sizeof(int));
                }
                std::memcpy(static_cast<char*>(data) + offset, ssbo.pData->data(), ssbo.pData->size() * sizeof(T));

                glBufferData(GL_SHADER_STORAGE_BUFFER, totalSize, data, GL_DYNAMIC_DRAW);
                std::free(data);
        } else {
                glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, oldBufferSize - offset, ssbo.pData->data());
        }

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo.binding, ssbo.bufferID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
