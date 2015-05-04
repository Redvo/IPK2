/****************************************************************************************
 * Project 1 in course Computer Communications and Networks, 2014/2015
 * Author: Lukáš Rendvanský
 * Email: xrendv00@stud.fit.vutbr.cz
 * Date: 18.3.2015
 * File: arguments.class.hpp
 * Description: Class storing arguments configuration - header file.
 ***************************************************************************************/

#ifndef ARG_CLASS
#define ARG_CLASS

#include <vector>	
#include <string>
#include <iostream>
#include <algorithm>
#include <limits>

#include "exception.class.hpp"

using namespace std;

#define MIN_PORT_NUMBER 1025
#define MAX_PORT_NUMBER 65535

#define MIN_PACKET_SIZE 10
#define MAX_PACKET_SIZE 65535

#define MIN_RATE 1
#define MAX_RATE numeric_limits<int>::max()

#define MIN_TIMEOUT 1
#define MAX_TIMEOUT numeric_limits<int>::max()

#define MIN_INTERVAL 1
#define MAX_INTERVAL numeric_limits<int>::max()

class Arguments {

	public:
		explicit Arguments(int argc, char** argv) {
			this->argumentCount = argc;
			this->arguments.insert(end(arguments), argv+1, argv+argc);
			this->helpSet = false;
			this->portNumber = 5656;
		};

		~Arguments() { };

		int GetArgumentCount();

		vector<string> GetArguments();

 		void PrintArguments();

 		bool IsHelpSet();

		int GetPortNumber();

	private:
		int argumentCount;
		vector<string> arguments;
		bool helpSet;
		int portNumber;

	protected:
		void SetPortNumber(int value);

		void SetHelp(bool value);

};

class ClientArguments: public Arguments {

	public:
		explicit ClientArguments(int argc, char** argv) : Arguments(argc,argv) {
			this->size = 100;
			this->rate = 10;
			this->timeout = 0;
			this->interval = static_cast<int>(round(100.0 / static_cast<double>(this->rate)));
			this->intervalSet = false;
		};

		virtual ~ClientArguments() {};

		enum SearchBy {
			LOGIN = 0, 
			UID = 1
		};

		void ParseArguments();

		string GetServerName();
		int GetPacketSize();
		int GetSendRate();
		int GetClientTimeout();
		int GetMeasurementInterval();
		bool GetIsIntervalSet();
		void SetMeasurementInterval(int value);
		void SetIsIntervalSet(bool value);

	private:
		string serverName;
		int size;
		int rate;
		int timeout;
		int interval;
		bool intervalSet;

		void SetServerName(string value);
		void SetPacketSize(int value);
		void SetSendRate(int value);
		void SetClientTimeout(int value);

};

class ServerArguments: public Arguments {

	public:
		explicit ServerArguments(int argc, char** argv) : Arguments(argc,argv) { };

		virtual ~ServerArguments() {};

		void ParseArguments();

};

#endif /* ARG_CLASS */