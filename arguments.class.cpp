/****************************************************************************************
 * Project 1 in course Computer Communications and Networks, 2014/2015
 * Author: Lukáš Rendvanský
 * Email: xrendv00@stud.fit.vutbr.cz
 * Date: 18.3.2015
 * File: arguments.class.cpp
 * Description: Class storing arguments configuration - both client and server sides.
 ***************************************************************************************/

#include "arguments.class.hpp"

using namespace std;

/************************************************************
******************** COMMON ARGUMENTS ***********************
************************************************************/

int Arguments::GetArgumentCount() {
	return this->argumentCount;
}

vector<string> Arguments::GetArguments() {
	return this->arguments;
}

void Arguments::PrintArguments() {
	for (vector<string>::iterator iterator = this->arguments.begin(); iterator != this->arguments.end(); ++iterator) {
    	cout << *iterator << endl;
	}	
}

bool Arguments::IsHelpSet() {
	return this->helpSet;
}

void Arguments::SetHelp(bool value) {
	this->helpSet = value;
}

void Arguments::SetPortNumber(int value) {
	this->portNumber = value;
}

int Arguments::GetPortNumber() {
	return this->portNumber;
}

/************************************************************
******************** SERVER ARGUMENTS ***********************
************************************************************/

void ServerArguments::ParseArguments() {

	int argCount = this->GetArgumentCount();	
	vector<string> arguments = this->GetArguments();

	int port = 0;
	string::size_type sz;

	if(argCount == 1) {

		return;

	} else if(argCount == 2 && arguments.front() == "--help") {

			this->SetHelp(true);
			return;

	} else if(argCount == 3 && (arguments.front() == "-p" || arguments.front() == "-port" )) {

			try {
				port = stoi(arguments.back(),&sz);
			} catch(...) {
				throw WrongArgumentsException("Invalid port number. Run with --help for detailed instructions.",
					Exception::WRONG_ARGUMENT);
			}

			if(sz != arguments.back().length() || port < MIN_PORT_NUMBER || port > MAX_PORT_NUMBER) {
				throw WrongArgumentsException("Invalid port number. Run with --help for detailed instructions.",
					Exception::WRONG_ARGUMENT);
			}

			this->SetPortNumber(port);

	} else {

		throw WrongArgumentsException("Invalid arguments. Run with --help for detailed instructions.",
			Exception::WRONG_ARGUMENT);

	}

	return;
	
}	

/************************************************************
******************** CLIENT ARGUMENTS ***********************
************************************************************/

void ClientArguments::SetServerName(string value) {
	this->serverName = value;
}

void ClientArguments::SetPacketSize(int value) {
	this->size = value;
}

void ClientArguments::SetSendRate(int value) {
	this->rate = value;
}

void ClientArguments::SetClientTimeout(int value) {
	this->timeout = value;
}

void ClientArguments::SetMeasurementInterval(int value) {
	this->interval = value;
}

void ClientArguments::SetIsIntervalSet(bool value) {
	this->intervalSet = value;
}

string ClientArguments::GetServerName() {
	return this->serverName;
}

int ClientArguments::GetPacketSize() {
	return this->size;
}

int ClientArguments::GetSendRate() {
	return this->rate;
}
	
int ClientArguments::GetClientTimeout() {
	return this->timeout;
}

int ClientArguments::GetMeasurementInterval() {
	return this->interval;
}

bool ClientArguments::GetIsIntervalSet() {
	return this->intervalSet;
}


void ClientArguments::ParseArguments() {

	int argCount = this->GetArgumentCount();	
	vector<string> arguments = this->GetArguments();

	int port = 0, size = 0, rate = 0, interval = 0, timeout = 0;
	string::size_type sz;
	string serverName = "";

	bool portSet = false;
	bool serverSet = false;
	bool sizeSet = false;
	bool rateSet = false;
	bool intervalSet = false;
	bool timeoutSet = false;	 
	

	if(argCount == 2 && (arguments.front() == "--help" || arguments.front() == "-h")) {

			this->SetHelp(true);
			return;

	} else {

		for(vector<string>::iterator iterator = begin(arguments); iterator != end(arguments); ++iterator) {

			// Set port number (-p port)
			if((*iterator == "-p" || *iterator == "--port") && ((iterator+1) != end(arguments))) {

				if(portSet) {
					throw WrongArgumentsException("Error: cannot set multiple ports. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);
				} else {
					portSet = true;
				}

				try {
					port = stoi(*(++iterator),&sz);
				} catch(...) {
					throw WrongArgumentsException("Invalid port number. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);
				}

				if(sz != (*iterator).length() || port < MIN_PORT_NUMBER || port > MAX_PORT_NUMBER) {
					throw WrongArgumentsException("Invalid port number. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);
				}

				this->SetPortNumber(port);

			// Set packet size (-s size)
			} else if((*iterator == "-s" || *iterator == "--size") && ((iterator+1) != end(arguments))) {

				if(sizeSet) {
					throw WrongArgumentsException("Error: can not set multiple packet sizes. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);
				} else {
					sizeSet = true;
				}

				try {
					size = stoi(*(++iterator),&sz);
				} catch(...) {
					throw WrongArgumentsException("Invalid packet size. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);
				}

				if(sz != (*iterator).length() || size < MIN_PACKET_SIZE || size > MAX_PACKET_SIZE) {
					throw WrongArgumentsException("Invalid packet size. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);
				}

				this->SetPacketSize(size);

			// Set packet send rate (-r rate)
			} else if((*iterator == "-r" || *iterator == "--rate") && ((iterator+1) != end(arguments))) {

				if(rateSet) {
					throw WrongArgumentsException("Error: can not set multiple send rates. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);
				} else {
					rateSet = true;
				}

				try {
					rate = stoi((*(++iterator)).c_str(),&sz,10);
				} catch(...) {
					throw WrongArgumentsException("Invalid rate. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);
				}

				if(sz != (*iterator).length() || rate < MIN_RATE || rate > MAX_RATE) {
					throw WrongArgumentsException("Invalid rate. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);	
				}

				this->SetSendRate(rate);

			// Set client timeout (-t timeout)
			} else if((*iterator == "-t" || *iterator == "--timeout") && ((iterator+1) != end(arguments))) {

				if(timeoutSet) {
					throw WrongArgumentsException("Error: can not set multiple timeouts. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);
				} else {
					timeoutSet = true;
				}

				try {
					timeout = stoi((*(++iterator)).c_str(),&sz);
				} catch(...) {
					throw WrongArgumentsException("Invalid timeout. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);
				}

				if(sz != (*iterator).length() || timeout < MIN_RATE || timeout > MAX_RATE) {
					throw WrongArgumentsException("Invalid timeout. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);	
				}

				this->SetClientTimeout(timeout);

			// Set measurement interval (-i interval)
			} else if((*iterator == "-i" || *iterator == "--interval") && ((iterator+1) != end(arguments))) {

				if(intervalSet) {
					throw WrongArgumentsException("Error: can not set multiple timeouts. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);
				} else {
					intervalSet = true;
				}

				try {
					interval = stoi((*(++iterator)).c_str(),&sz);
				} catch(...) {
					throw WrongArgumentsException("Invalid timeout. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);
				}

				if(sz != (*iterator).length() || interval < MIN_RATE || interval > MAX_RATE) {
					throw WrongArgumentsException("Invalid timeout. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);	
				}

				this->SetMeasurementInterval(interval);
				this->SetIsIntervalSet(true);

			} else {

				if(serverSet) {
					throw WrongArgumentsException("Invalid arguments. Run with --help for detailed instructions.",
						Exception::WRONG_ARGUMENT);
				}

				this->SetServerName(*iterator);
				serverSet = true;

			}

		}

		if(!serverSet) {
			throw WrongArgumentsException("Invalid arguments. Run with --help for detailed instructions.",
				Exception::WRONG_ARGUMENT);
		}

	}

	return;

}

	