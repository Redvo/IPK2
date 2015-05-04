/****************************************************************************************
 * Project 1 in course Computer Communications and Networks, 2014/2015
 * Author: Lukáš Rendvanský
 * Email: xrendv00@stud.fit.vutbr.cz
 * Date: 18.3.2015
 * File: server.cpp
 * Description: Implementation of server-side application - source code.
 ***************************************************************************************/

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "arguments.class.hpp"
#include "connection.class.hpp"
#include "exception.class.hpp"

using namespace std;

const string helpMessage = "\nProject 2 in course Computer Communications and Networks, 2014/2015\n\
Author: Lukas Rendvansky (xrendv00@stud.fit.vutbr.cz)\n\n\
Description: Server side implementation. This program test parameters of end to end connection.\n\n\
Usage: ./server [-p PORT]\n\
  PORT - Port number on which server will run, in range of 1025 to 65535. Implicit value is 5656.\n";

// Macro to get rid of unused parameter warnings
#define UNUSED(x) (void)(x)

void AlarmHandler(int sig) {
	UNUSED(sig);
	signal(SIGALRM, SIG_IGN); // Ignore this signal while we handle it
	throw TimeoutException("Waiting for message timed out.", Exception::TIMEOUT);
}

void InterruptHandler(int sig) {
	UNUSED(sig);
	signal(SIGINT, SIG_IGN); // Ignore this signal while we handle it
	throw InterruptException("Connection to server timed out.", Exception::TIMEOUT);
}

int main(int argc, char** argv)
{
	// Set AlarmHandler() function to handle SIGALRM signal for timeout
	signal(SIGALRM, AlarmHandler);
	//signal(SIGINT, InterruptHandler);

	// Create new object and try to parse client specific arguments 
	ServerArguments args(argc,argv);
	try {
		args.ParseArguments();
	} catch(WrongArgumentsException ex) {
		cerr << ex.getMessage() << endl;
		exit(ex.getCode());
	}	

	// Print help if requested
	if(args.IsHelpSet()) {
		cout << helpMessage << endl;
		return 0;
	}

	ServerConnection connection(args.GetPortNumber());

	// Create new connection object with port number specified in arguments
	
	try {
		connection.CreateSocket(); // Create new socket for server
		connection.Bind(); // Bind this socket
	} catch (ConnectionException ex){
		cerr << ex.getMessage() << endl;
		exit(ex.getCode());
	}

	// Infinite loop for accepting and handling new connections from clients
	while(1) {

		try {
			T_Packet *request = connection.ReadRequest();
			connection.SendResponse(request);
		} catch(...) {
			continue;
		}

	}
	
	// Close server
	try {
		connection.Close();
	} catch(ConnectionException ex) {
		exit(ex.getCode());
	}	

	exit(0);

}

/* End of file server.cpp */
