#include "Shader.h"

#include "glad/glad.h"
#include "glm/glm.hpp"

#define OPENGL_MESSAGE_BUFFER_SIZE 512

Shader::Shader(const std::string_view &vertexCode, const std::string_view &fragmentCode)
{
        this->compileShader(vertexCode.data(), GL_VERTEX_SHADER, m_vertexID);
        this->compileShader(fragmentCode.data(), GL_FRAGMENT_SHADER, m_fragmentID);
        this->linkShaders();

        this->use();
        this->initFullScreenQuadVertices();
}

void Shader::compileShader(const char* code, GLenum type, uint32_t &id)
{
        id = glCreateShader(type);
        glShaderSource(id, 1, &code, nullptr);
        glCompileShader(id);

        int32_t success{};
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success) {
                char infoLog[OPENGL_MESSAGE_BUFFER_SIZE];
                glGetShaderInfoLog(id, OPENGL_MESSAGE_BUFFER_SIZE, nullptr, infoLog);
                std::cout << "Shader compilation failed! " << infoLog << std::endl;
        }
}

void Shader::linkShaders()
{
        m_programID = glCreateProgram();
        glAttachShader(m_programID, m_vertexID);
        glAttachShader(m_programID, m_fragmentID);
        glLinkProgram(m_programID);

        int32_t success{};
        glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
        if (!success) {
                char infoLog[OPENGL_MESSAGE_BUFFER_SIZE];
                glGetShaderInfoLog(m_programID, OPENGL_MESSAGE_BUFFER_SIZE, nullptr, infoLog);
                std::cout << "Shader compilation failed! " << infoLog << std::endl;
        }

        glDeleteShader(m_vertexID);
        glDeleteShader(m_fragmentID);
}

void Shader::initFullScreenQuadVertices()
{
        glm::vec2 vertices[4] { glm::vec2(1.0), glm::vec2(1.0, -1.0), glm::vec2(-1.0), glm::vec2(-1.0, 1.0) };
        uint32_t indices[6]   { 0, 1, 3, 1, 2, 3 };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers     (1, &m_VBO);
        glGenBuffers     (1, &m_EBO);

        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

void Shader::use() const
{
        glUseProgram(m_programID);
}

void Shader::drawFullScreenQuad() const
{
        this->use();

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
}