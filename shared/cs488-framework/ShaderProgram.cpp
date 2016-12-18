#include "ShaderProgram.hpp"
#include "ShaderException.hpp"
#include "GlErrorCheck.hpp"

#include <glm/gtc/type_ptr.hpp>
using glm::value_ptr;

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

//------------------------------------------------------------------------------------
ShaderProgram::Shader::Shader()
    : shaderObject(0),
      filePath()
{

}

//------------------------------------------------------------------------------------
ShaderProgram::ShaderProgram()
        : programObject(0),
          prevProgramObject(0),
          activeProgram(0)
{

}
//------------------------------------------------------------------------------------
void ShaderProgram::generateProgramObject() {
    if(programObject == 0) {
        programObject = glCreateProgram();
    }
}

//------------------------------------------------------------------------------------
void ShaderProgram::attachVertexShader (
		const char * filePath
) {
    vertexShader.shaderObject = createShader(GL_VERTEX_SHADER);
    vertexShader.filePath = filePath;

    extractSourceCodeAndCompile(vertexShader);
}

//------------------------------------------------------------------------------------
void ShaderProgram::attachFragmentShader (
		const char * filePath
) {
    fragmentShader.shaderObject = createShader(GL_FRAGMENT_SHADER);
    fragmentShader.filePath = filePath;

    extractSourceCodeAndCompile(fragmentShader);
}

//------------------------------------------------------------------------------------
void ShaderProgram::attachGeometryShader (
		const char * filePath
) {
    geometryShader.shaderObject = createShader(GL_GEOMETRY_SHADER);
    geometryShader.filePath = filePath;

    extractSourceCodeAndCompile(geometryShader);
}

//------------------------------------------------------------------------------------
void ShaderProgram::extractSourceCodeAndCompile (
		const Shader & shader
) {
    string shaderSourceCode;
    extractSourceCode(shaderSourceCode, shader.filePath);

    compileShader(shader.shaderObject, shaderSourceCode);
}

//------------------------------------------------------------------------------------
void ShaderProgram::recompileShaders() {
    extractSourceCodeAndCompile(vertexShader);
    extractSourceCodeAndCompile(fragmentShader);
    extractSourceCodeAndCompile(geometryShader);
}

//------------------------------------------------------------------------------------
/*
* Extracts source code from file located at 'filePath' and places contents into
* 'shaderSource'.
*/
void ShaderProgram::extractSourceCode (
		string & shaderSource,
		const string & filePath
) {
    ifstream file;

    file.open(filePath.c_str());
    if (!file) {
        stringstream strStream;
        strStream << "Error -- Failed to open file: " << filePath << endl;
        throw ShaderException(strStream.str());
    }

    stringstream strBuffer;
    string str;

    while(file.good()) {
        getline(file, str, '\r');
        strBuffer << str;
    }
    file.close();

    strBuffer << '\0';  // Append null terminator.

    shaderSource = strBuffer.str();
}

//------------------------------------------------------------------------------------
/*
* Links all attached shaders within the ShaderProgram.
* Note: This method must be called once before calling ShaderProgram::enable().
*/
void ShaderProgram::link() {
    if(vertexShader.shaderObject != 0) {
        glAttachShader(programObject, vertexShader.shaderObject);
    }

    if(fragmentShader.shaderObject != 0) {
        glAttachShader(programObject, fragmentShader.shaderObject);
    }

    if(geometryShader.shaderObject != 0) {
        glAttachShader(programObject, geometryShader.shaderObject);
    }

    glLinkProgram(programObject);
    checkLinkStatus();

    CHECK_GL_ERRORS;
}

//------------------------------------------------------------------------------------
ShaderProgram::~ShaderProgram() {
    deleteShaders();
}

//------------------------------------------------------------------------------------
void ShaderProgram::deleteShaders() {
    glDeleteShader(vertexShader.shaderObject);
    glDeleteShader(fragmentShader.shaderObject);
    glDeleteProgram(programObject);
}

//------------------------------------------------------------------------------------
GLuint ShaderProgram::createShader (
		GLenum shaderType
) {
    GLuint shaderId = glCreateShader(shaderType);
    CHECK_GL_ERRORS;

    return shaderId;
}

//------------------------------------------------------------------------------------
void ShaderProgram::compileShader (
		GLuint shaderObject,
		const string & shaderSourceCode
) {
    const char * sourceCodeStr = shaderSourceCode.c_str();
    glShaderSource(shaderObject, 1, (const GLchar **)&sourceCodeStr, NULL);

    glCompileShader(shaderObject);
    checkCompilationStatus(shaderObject);

    CHECK_GL_ERRORS;
}

//------------------------------------------------------------------------------------
void ShaderProgram::checkCompilationStatus (
		GLuint shaderObject
) {
    GLint compileSuccess;

    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE) {
        GLint errorMessageLength;
        // Get the length in chars of the compilation error message.
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &errorMessageLength);

        // Retrieve the compilation error message.
        GLchar errorMessage[errorMessageLength + 1]; // Add 1 for null terminator
        glGetShaderInfoLog(shaderObject, errorMessageLength, NULL, errorMessage);

        string message = "Error Compiling Shader: ";
        message += errorMessage;

        throw ShaderException(message);
    }
}

//------------------------------------------------------------------------------------
void ShaderProgram::enable() const {
    glUseProgram(programObject);
    CHECK_GL_ERRORS;
}

//------------------------------------------------------------------------------------
void ShaderProgram::disable() const {
    glUseProgram((GLuint)NULL);
    CHECK_GL_ERRORS;
}

//------------------------------------------------------------------------------------
void ShaderProgram::checkLinkStatus() {
    GLint linkSuccess;

    glGetProgramiv(programObject, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLint errorMessageLength;
        // Get the length in chars of the link error message.
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &errorMessageLength);

        // Retrieve the link error message.
        GLchar errorMessage[errorMessageLength];
        glGetProgramInfoLog(programObject, errorMessageLength, NULL, errorMessage);

        stringstream strStream;
        strStream << "Error Linking Shaders: " << errorMessage << endl;

        throw ShaderException(strStream.str());
    }
}

//------------------------------------------------------------------------------------
/*
 * Returns the GL shader program object name.
 */
GLuint ShaderProgram::getProgramObject() const {
    return programObject;
}

//------------------------------------------------------------------------------------
/*
 * Returns the location value of a uniform variable within the shader program.
 */
GLint ShaderProgram::getUniformLocation (
		const char * uniformName
) const {
    GLint result = glGetUniformLocation(programObject, (const GLchar *)uniformName);

    if (result == -1) {
        stringstream errorMessage;
        errorMessage << "Error obtaining uniform location: " << uniformName;
        throw ShaderException(errorMessage.str());
    }

    return result;
}

//------------------------------------------------------------------------------------
/*
 * Returns the location value of an attribute variable within the shader program.
 */
GLint ShaderProgram::getAttribLocation (
		const char * attributeName
) const {
    GLint result = glGetAttribLocation(programObject, (const GLchar *)attributeName);

    if (result == -1) {
        stringstream errorMessage;
        errorMessage << "Error obtaining attribute location: " << attributeName;
        throw ShaderException(errorMessage.str());
    }

    return result;
}

