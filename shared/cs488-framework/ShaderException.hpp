/**
* ShaderException
*/

#pragma once

#include <exception>
#include <string>
using std::string;

/*
 * Exception class for encapsulating ShaderProgram error messages.
 */
class ShaderException : public std::exception {
public:
    ShaderException(const string & message)
        : errorMessage(message) { }

    virtual ~ShaderException() noexcept { }

    virtual const char * what() const noexcept {
        return errorMessage.c_str();
    }

private:
    string errorMessage;
};
