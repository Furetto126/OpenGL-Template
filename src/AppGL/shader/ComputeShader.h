#pragma once

#include <memory>
#include "Shader.h"

class ComputeShader
{
public:
        uint32_t localSizeX = 1, localSizeY = 1, localSizeZ = 1;

        ComputeShader(const char* computeShaderCode, const char* vertexShaderCode, const char* fragmentShaderCode);

        void drawFullScreenQuad(const uint32_t width, const uint32_t height, const uint32_t binding);
        void use() const;

        template<typename T>
        void addSSBO(SSBO<T>& ssbo);
        template<typename T>
        void setSSBO(SSBO<T>& ssbo);

        template<typename T>
        void addUniform(const std::string &name, const T &value);
        template<typename T>
        void setUniform(const std::string &name, const T &value);
private:
        uint32_t programID{};

        std::shared_ptr<Shader> renderShader;
        uint32_t texture{};
        uint32_t texWidth{}, texHeight{};

        std::vector<std::string> initializedUniforms{};

        uint32_t nextFreeSSBOBinding{};
        std::vector<SSBO<std::any>> SSBOs{};

        void initDrawToTexture(const uint32_t width, const uint32_t height, const uint32_t binding);
};

template<typename T>
void ComputeShader::addUniform(const std::string &name, const T &value)
{
        const bool isPresent = std::any_of(initializedUniforms.begin(), initializedUniforms.end(), [name](const std::string &u) -> bool {
                return u == name;
        });

        if (isPresent)
                std::cout << "Cannot add uniform as it was already initialized. Name: " << name << std::endl;

        initializedUniforms.push_back(name);

        this->use();
        int uniformLocation = glGetUniformLocation(programID, name.data());

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
void ComputeShader::setUniform(const std::string &name, const T &value)
{
        const bool isPresent = std::any_of(initializedUniforms.begin(), initializedUniforms.end(), [name](const std::string &u) -> bool {
                return u == name;
        });

        if (!isPresent)
                std::cout << "Cannot set uniform as it was not already initialized. Name: " << name << std::endl;

        this->use();
        const int location = glGetUniformLocation(programID, name.data());

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
void ComputeShader::addSSBO(SSBO<T> &ssbo)
{
        if (ssbo.binding != nextFreeSSBOBinding)
                std::cout << "Cannot initialize SSBO in a non-increasing order.\nBinding '" << ssbo.binding <<
                        "' with next free binding '" << nextFreeSSBOBinding << "'" << std::endl;

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
        nextFreeSSBOBinding++;
}

template<typename T>
void ComputeShader::setSSBO(SSBO<T> &ssbo)
{
        if (ssbo.binding >= nextFreeSSBOBinding)
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
