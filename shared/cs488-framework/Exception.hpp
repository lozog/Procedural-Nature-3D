#pragma once

#include <exception>
#include <string>

/*
 * Exception class for encapsulating error messages during runtime.
 */
class Exception : public std::exception {
public:
	// Copy of the message.
	Exception(std::string message)
			: errorMessage(message) { }

	virtual ~Exception() noexcept { }

	virtual const char * what() const noexcept {
		return errorMessage.c_str();
	}

private:
	std::string errorMessage;

};
