/****************************************************************************************
 * Project 1 in course Computer Communications and Networks, 2014/2015
 * Author: Lukáš Rendvanský
 * Email: xrendv00@stud.fit.vutbr.cz
 * Date: 18.3.2015
 * File: connection.class.hpp
 * Description: Implementation of connection class - header file.
 ***************************************************************************************/

#ifndef CONNECTION_H
#define CONNECTION_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <poll.h>
#include "exception.class.hpp"

#define PACKET_HEADER_SIZE (sizeof(struct Packet) - sizeof(char*))

typedef struct Packet {
	
	/* Packet header */
	uint16_t PACKET_SZ;    // for packet size         
	uint32_t SEQ_NUM;      // sequence number     
	struct timeval TIME;   // timestamp

	/* Packet data */
	unsigned char *data;

} T_Packet;

typedef struct Counter {
	unsigned int sent;              // Number of packets sent to server
	unsigned int received;          // Number of packets received back
	unsigned int inOrder;           // Number of packets that arrived in order
	unsigned int outOfOrder;        // Number of packets that arrived out of order
	unsigned int bytesSent;         // Number of total bytes sent
	unsigned int bytesReceived;     // Number of total bytes received
	double minTravelTime; // Time (in ms) of fastest response from server
	double maxTravelTime;  // Time (in ms) of slowest response from server
	double RTT;
} T_Counter;

class Connection {

public:
	Connection(int port) {
		this->port = port;
	};

	virtual ~Connection() { };

	int GetPort();

	int GetSocket();

protected:

	int port;
	int soc;
	socklen_t sinlen, soutlen;
	struct sockaddr_in socketAddressIn, socketAddressOut;

};

class ServerConnection: public Connection {

public:
	ServerConnection(int port): Connection(port) {	};

	virtual ~ServerConnection() { };

	void CreateSocket();

	void Bind();

	T_Packet *ReadRequest();

	void SendResponse(T_Packet *value);

	void Close();

};

class ClientConnection: public Connection {

public:
	ClientConnection(int port): Connection(port) {	};

	virtual ~ClientConnection() { };

	void SetHostName(std::string value);

	std::string GetHostName();

	void CreateSocket();

	void Connect();

	void SendRequest(T_Packet *value, T_Counter *counter);

	T_Packet *ReadResponse(T_Counter *counter, int interval);

	void Close();

private:

	struct hostent *hptr;

	std::string hostname;

};


#endif /* CONNECTION_H */