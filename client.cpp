/****************************************************************************************
 * Project 1 in course Computer Communications and Networks, 2014/2015
 * Author: Lukáš Rendvanský
 * Email: xrendv00@stud.fit.vutbr.cz
 * Date: 18.3.2015
 * File: client.cpp
 * Description: Implementation of client-side application - source code.
 ***************************************************************************************/

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <ctime>
#include <cmath>
#include <climits>
#include <signal.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/time.h>
#include "arguments.class.hpp"
#include "connection.class.hpp"
#include "exception.class.hpp"

using namespace std;

const string helpMessage = "\nProject 2 in course Computer Communications and Networks, 2014/2015\n\
Author: Lukas Rendvansky (xrendv00@stud.fit.vutbr.cz)\n\n\
Description: Client side implementation. This program test parameters of end to end connection.\n\n\
Usage: ./server [-p | --port PORT] [-s | --size SIZE] [-r | --rate RATE] [-i | --interval INTERVAL] [-t | --timeout TIMEOUT]\n\
  PORT - Port number on which server will run, in range of 1025 to 65535. Implicit value is 5656.\n\
  SIZE - Size of packet payload. Minimum value 10, implicit 100.\n\
  RATE - Number of packets sent per second. Minimum value 1, implicit value 10.\n\
  INTERVAL - Time in seconds in which values are measured. Minimum 1, implicit 100/rate.\n\
  TIMEOUT - Time in seconds after client automatically closes. If not set, runs infinitely.\n\n";

// Macro to get rid of unused parameter warnings
#define UNUSED(x) (void)(x)

// Macro that computes rounded time difference in milliseconds
#define TIME_DIFF(time1, time2) (((((double)(time2.tv_sec - time1.tv_sec))*1000000) +\
  									((double)(time2.tv_usec - time1.tv_usec))) / 1000.0)

void AlarmHandler(int sig) {
	UNUSED(sig);
	signal(SIGALRM, SIG_IGN); // Ignore this signal while we handle it
	throw TimeoutException("Connection to server timed out.", Exception::TIMEOUT);
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
	signal(SIGINT, InterruptHandler);

	ClientConnection *connection = NULL;
	int senderPID, receiverPID;
	T_Counter *counter = NULL;
	T_Packet *packet = (T_Packet*) malloc(sizeof(struct Packet)); // Defined in connection.class.hpp
	unsigned int programStart, receiverStart, senderStart;
	int shm_id = -1;
	ofstream myfile;
	string filename;
	time_t rawtime;
  	struct tm* timeinfo;
	char timeBuffer [80];
	double outOfOrderPercentage;
	struct timeval tv;


	// Create new object and try to parse client specific arguments 
	ClientArguments args(argc,argv);
	try {
		args.ParseArguments();
	} catch(WrongArgumentsException ex) {
		cerr << ex.getMessage() << endl;
		exit(ex.getCode());
	}	

	if(!args.GetIsIntervalSet()) {
		args.SetMeasurementInterval(static_cast<int>(round(100.0 / static_cast<double>(args.GetSendRate()))));
	}

	//printf("Port: %d\nSize: %d\nRate: %d\nInterval: %d\nTimeout: %d\n",
	//	args.GetPortNumber(), args.GetPacketSize(), args.GetSendRate(),
	//		args.GetMeasurementInterval(), args.GetClientTimeout());

	packet->PACKET_SZ = static_cast<uint16_t>(args.GetPacketSize()) + PACKET_HEADER_SIZE;
	// Allocate packet size
	if(args.GetPacketSize() > 10) {
		packet->data = (unsigned char*) malloc((args.GetPacketSize()) * sizeof(char));
		memset(packet->data,255,(args.GetPacketSize()));
	}

	// Print help if requested
	if(args.IsHelpSet()) {
		cout << helpMessage << endl;
		return 0;
	}

	programStart = static_cast<unsigned>(time(NULL));

	filename = "ipkperf-" + args.GetServerName() + "-" + to_string(args.GetPacketSize()) + "-" + to_string(args.GetSendRate()); 

	try {
		myfile.open(filename, ios::in | ios::ate | ios::app | ios::binary);
	} catch (...) {
		cerr << "Error opening file." << endl;
		exit (1);
	}

	try {

	while(1) {

		shm_id = shmget(IPC_PRIVATE, sizeof(struct Counter), IPC_CREAT | IPC_EXCL | 0666);
		if(shm_id == -1) {
	        cerr << "Error opening shared memory." << string(strerror(errno)) << endl;
	        return 1;
	    }
		counter = (T_Counter *) shmat(shm_id, NULL, SHM_RND);
		if(counter == (void*) -1) {
			cerr << "Error creating shared segment." << endl;
		}



		counter->sent = 0;
		counter->received = 0;
		counter->inOrder = 0;
		counter->outOfOrder = 0;
		counter->bytesSent = 0;
		counter->bytesReceived = 0;
		counter->minTravelTime = INT_MAX;
		counter->maxTravelTime = 0;

		if((receiverPID = fork()) < 0) {

		} else if (receiverPID == 0) {

			// Create new connection object with port number and server address (domain name or IP)
			// specified in arguments
			connection = new ClientConnection(args.GetPortNumber());
			connection->SetHostName(args.GetServerName());

			// Create new socket and try to connect to server
			connection->CreateSocket();

			if((senderPID = fork()) < 0) {

			} else if (senderPID == 0) {
				
				/**
				 *	SENDER
				 */

				//cout << "Started new sender process" << endl;
				senderStart = static_cast<unsigned>(time(NULL));

				gettimeofday(&tv, NULL); 

				for(int i = 1; i <= (args.GetSendRate() * args.GetMeasurementInterval()); i++) {
					if(senderStart + args.GetMeasurementInterval() <= static_cast<unsigned>(time(NULL))) {
						break;
					}

					gettimeofday(&tv, NULL); 

					packet->SEQ_NUM = i;
					packet->TIME = tv;
					connection->SendRequest(packet, counter);
					counter->sent++;
				}

				connection->Close();
				exit(0);

			} else {

					/**
					 *	RECEIVER
					 */

					//cout << "Started new receiver process" << endl;
					unsigned int lastSeqNumber = 0;
					double rtt = 0.0, jitter = 0.0, oldRtt = 0.05;

					receiverStart = static_cast<unsigned>(time(NULL));
					time (&rawtime);
					timeinfo = localtime (&rawtime);
					strftime (timeBuffer, sizeof(timeBuffer), "%Y-%m-%d-%T", timeinfo);

					while(1) {


						if(receiverStart + args.GetMeasurementInterval() <= static_cast<unsigned>(time(NULL))) {
							break;
						}

						//cout << "RECEIVING" << endl;

						// read response from server
						try {
							//cout << "InOrder" << endl;
							packet = connection->ReadResponse(counter, args.GetMeasurementInterval());

							gettimeofday(&tv, NULL); 

							if(packet->SEQ_NUM > lastSeqNumber) {
								lastSeqNumber = packet->SEQ_NUM;
							} else {
								counter->outOfOrder++;
							}

							double timeDiff = TIME_DIFF(packet->TIME, tv);

							if(timeDiff < counter->minTravelTime) {
								counter->minTravelTime = timeDiff;
							}

							if (timeDiff > counter->maxTravelTime) {
								counter->maxTravelTime = timeDiff;
							}

							rtt = (0.115 * oldRtt) + ((1.0 - 0.115) * timeDiff);
							oldRtt = rtt;

							counter->received++;
							
						} catch (TimeoutException ex) {
							continue;
						}
					}

					if(lastSeqNumber == 0) {
						outOfOrderPercentage = 0.0;
						rtt = 0.0;
						jitter = 0;
					} else {

						if(counter->sent != 0) {
							outOfOrderPercentage = (static_cast<double>(counter->outOfOrder) * 100.0) / static_cast<double>(counter->received);
						} else {
							outOfOrderPercentage = 0.0;
						}

						jitter = counter->maxTravelTime - counter->minTravelTime;

					}

					myfile << string(timeBuffer) << ", " << to_string(args.GetMeasurementInterval()) << ", " <<
						to_string(counter->sent) << ", " << to_string(counter->received) << ", " <<
						to_string(counter->bytesSent) << ", " << to_string(counter->bytesReceived) << ", " <<
						setprecision(3) << setfill ('0') << setw(5) << fixed << rtt << "ms" << ", " << 
						setprecision(3) << setfill ('0') << setw(5) << fixed << jitter << "ms" <<  ", " << 
						setprecision(3) << setfill ('0') << setw(5) << fixed << outOfOrderPercentage << "%" << endl;

					if (shmdt(counter)==-1) {
				        cerr << "Unable to deallocate shared memory. Details: " << string(strerror(errno)) << endl;
				    }

				    if (shmctl(shm_id, IPC_RMID, NULL)==-1) {
				        cerr << "Unable to deallocate shared memory. Details: " << string(strerror(errno)) << endl;
				    }

					free(packet->data);
					free(packet);
					exit(0);

			}


		} else {

			sleep(args.GetMeasurementInterval());
			
			if(args.GetClientTimeout() != 0) {
				if(programStart + args.GetClientTimeout() <= static_cast<unsigned>(time(NULL))) {
					exit(0);
				}
			}

		}

	}

	// Handle all possible errors
	}catch (ConnectionException ex){
		cerr << ex.getMessage() << endl;
		exit(ex.getCode());
	} catch(InterruptException exc) {
		exit(0);
	}

}