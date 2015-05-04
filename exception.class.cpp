/****************************************************************************************
 * Project 1 in course Computer Communications and Networks, 2014/2015
 * Author: Lukáš Rendvanský
 * Email: xrendv00@stud.fit.vutbr.cz
 * Date: 18.3.2015
 * File: exception.class.cpp
 * Description: Implementation of exception class - source code.
 ***************************************************************************************/

#include "exception.class.hpp"

std::string Exception::getMessage() {
	return this->message;
}

int Exception::getCode() {
	return this->code; 
}
