#pragma once

#include <string>
#include <memory>
#include <glad/gl.h>
#include <exception>

class ShaderProgram {
private:
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;
    std::string vertexCode;
    std::string fragmentCode;

    const std::string LoadFromFile(const std::string &filename);
    void compileShader(GLuint shader, const char *code);
public:
    ShaderProgram();
    ~ShaderProgram();
    void CompileShaders();
    void Use();
    GLint GetUniformLocation(const char *uniformName);
    GLint GetAttributeLocation(const char *attrName);

    // Aliases
    void LoadVertexFile(const std::string &filename) { vertexCode = LoadFromFile(filename); }
    void LoadFragmentFile(const std::string &filename) { fragmentCode = LoadFromFile(filename); };
    GLint GetUniformLocation(const std::string &uniformName) { return GetUniformLocation(uniformName.c_str()); }
    GLint GetAttributeLocation(const std::string &attrName) { return GetAttributeLocation(attrName.c_str()); }
};

class ShaderException: public std::exception {
public:
    std::string msg;
    ShaderException(const std::string &msg) : msg(msg) { }
    virtual const char* what() const throw() {
        return msg.c_str();
    }
};

std::shared_ptr<ShaderProgram> MakeShaderProgram();