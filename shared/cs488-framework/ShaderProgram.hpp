/*
 * ShaderProgram
 */

#pragma once

#include "OpenGLImport.hpp"

#include <string>


class ShaderProgram {
public:
    ShaderProgram();

    ~ShaderProgram();

    void generateProgramObject();

    void attachVertexShader(const char * filePath);
    
    void attachFragmentShader(const char * filePath);
    
    void attachGeometryShader(const char * filePath);

    void link();

    void enable() const;

    void disable() const;

    void recompileShaders();

    GLuint getProgramObject() const;

    GLint getUniformLocation(const char * uniformName) const;

    GLint getAttribLocation(const char * attributeName) const;


private:
    struct Shader {
        GLuint shaderObject;
        std::string filePath;

        Shader();
    };

    Shader vertexShader;
    Shader fragmentShader;
    Shader geometryShader;

    GLuint programObject;
    GLuint prevProgramObject;
    GLuint activeProgram;

    void extractSourceCode(std::string & shaderSource, const std::string & filePath);
    
    void extractSourceCodeAndCompile(const Shader &shader);

    GLuint createShader(GLenum shaderType);

    void compileShader(GLuint shaderObject, const std::string & shader);

    void checkCompilationStatus(GLuint shaderObject);

    void checkLinkStatus();

    void deleteShaders();
};

