#include "shader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

const std::string ShaderProgram::LoadFromFile(const std::string &filename) {
    std::ifstream f;
    std::string result;
    f.open (filename);
    if (f.is_open()) {
        std::ostringstream sstr;
        sstr << f.rdbuf();
        f.close();
        return sstr.str();
    } else {
        throw ShaderException("cannot open file: " + filename);
    }
}

ShaderProgram::ShaderProgram() : vertexShader(0), fragmentShader(0), program(0) {}

ShaderProgram::~ShaderProgram() {
    if (program) {
        glDeleteProgram(program);
    }
    if (fragmentShader) {
        glDeleteShader(fragmentShader);
    }
    if (vertexShader) {
        glDeleteShader(vertexShader);
    }
}

void ShaderProgram::compileShader(GLuint shader, const char *code) {
    GLint status = 0;
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
        std::string errormsg(errorLog.begin(), errorLog.end());
        throw ShaderException(errormsg);
    }
}

void ShaderProgram::CompileShaders() {
    GLint isCompiled = 0;
    auto vertexCodePtr = vertexCode.c_str();
    auto fragmentCodePtr = fragmentCode.c_str();

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    try {
        compileShader(vertexShader, vertexCodePtr);
    } catch (ShaderException &excp) {
        glDeleteShader(vertexShader);
        vertexShader = 0;
        throw ShaderException("error compiling vertex shader:" + excp.msg);
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    try {
        compileShader(fragmentShader, fragmentCodePtr);
    } catch (ShaderException &excp) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        vertexShader = 0;
        fragmentShader = 0;
        throw ShaderException("error compiling fragment shader:" + excp.msg);
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint Result = GL_FALSE;
    GLint maxLength = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &Result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
	if (Result == GL_FALSE) {
		std::vector<char> errorLog(maxLength);
		glGetProgramInfoLog(program, maxLength, NULL, &errorLog[0]);
        std::string errormsg(errorLog.begin(), errorLog.end());
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        vertexShader = 0;
        fragmentShader = 0;
        program = 0;
        throw ShaderException("error linking shader:" + errormsg);
	}

    glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
    vertexShader = 0;
    fragmentShader = 0;
}

void ShaderProgram::Use() {
    glUseProgram(program);
}

GLint ShaderProgram::GetUniformLocation(const char *uniformName) {
    return glGetUniformLocation(program, uniformName);
}

GLint ShaderProgram::GetAttributeLocation(const char *attrName) {
    return glGetAttribLocation(program, attrName);
}

std::shared_ptr<ShaderProgram> MakeShaderProgram() {
    return std::make_shared<ShaderProgram>();
}