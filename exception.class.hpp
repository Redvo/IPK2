/****************************************************************************************
 * Project 1 in course Computer Communications and Networks, 2014/2015
 * Author: Lukáš Rendvanský
 * Email: xrendv00@stud.fit.vutbr.cz
 * Date: 18.3.2015
 * File: exception.class.hpp
 * Description: Implementation of exception class - heder file.
 ***************************************************************************************/

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <exception>

 class Exception : public std::exception
{
	public:
		Exception(std::string message, int code) {
			this->message = message;
			this->code = code;
		}

		~Exception() throw() {}

		enum ExceptionCodes {
			// Network exceptions
			INVALID_SOCKET = 10,
			BIND_FAILED = 11,
			LISTEN_FAILED = 12,
			ACCEPT_FAILED = 13,
			CONNECT_FAILED = 14,

			READ_REQUEST_FAILED = 15,
			SEND_RESPONSE_FAILED = 16,

			SEND_REQUEST_FAILED = 17,
			READ_RESPONSE_FAILED = 18,

			CLOSE_CONNECTION = 19, 
			CLOSE_SERVER = 20,
			CLOSE_CLIENT = 21,

			INVALID_RESPONSE = 22,
			INVALID_REQUEST = 23,

			TIMEOUT = 25,

			// Multiprocessing
			FORK_FAILED = 30,

			// Searching
			INVALID_PASSWD_READ = 40,

			// Argument exceptions
			WRONG_ARGUMENT = 90
		};

		std::string getMessage();

		int getCode();

	private:
		std::string message;
		int code;
};

class WrongArgumentsException : public Exception
{
	public:
		WrongArgumentsException(std::string message, int code) : Exception(message, code) {}

		~WrongArgumentsException() throw() {}

};

class ConnectionException : public Exception
{
	public:
		ConnectionException(std::string message, int code) : Exception(message, code) {}

		~ConnectionException() throw() {}
};

class SearcherException : public Exception
{
	public:
		SearcherException(std::string message, int code) : Exception(message, code) {}

		~SearcherException() throw() {}
};

class TimeoutException : public Exception
{
	public:
		TimeoutException(std::string message, int code) : Exception(message, code) {}

		~TimeoutException() throw() {}
};

class InterruptException : public Exception
{
	public:
		InterruptException(std::string message, int code) : Exception(message, code) {}

		~InterruptException() throw() {}
};

#endif /* EXCEPTION_H */