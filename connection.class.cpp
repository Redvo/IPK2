/***************************************************************************************
 * Project 1 in course Computer Communications and Networks, 2014/2015
 * Author: Lukáš Rendvanský
 * Email: xrendv00@stud.fit.vutbr.cz
 * Date: 18.3.2015
 * File: connection.class.cpp
 * Description: Implementation of connection class - source code.
 ***************************************************************************************/

#include "connection.class.hpp"
#include "arguments.class.hpp"

int Connection::GetPort() {
	return this->port;
}

int Connection::GetSocket() {
	return this->soc;
}

void ServerConnection::CreateSocket() {
	if ((this->soc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { // create socket
		throw ConnectionException("Socket creation failed.",Exception::INVALID_SOCKET);
	}
}

void ServerConnection::Bind() {


	memset((char *)&this->socketAddressIn, 0, sizeof(this->socketAddressIn));
	this->socketAddressIn.sin_family = AF_INET;
	this->socketAddressIn.sin_addr.s_addr = htonl(INADDR_ANY);
	this->socketAddressIn.sin_port = htons(this->port);

	// bind socket
	if (bind(this->soc, (struct sockaddr*) &this->socketAddressIn, sizeof(this->socketAddressIn)) < 0 ) {
		throw ConnectionException("Binding of socket failed.",Exception::BIND_FAILED);
	}

	this->sinlen = sizeof(this->socketAddressIn);
	this->soutlen = sizeof(this->socketAddressOut);

}

T_Packet *ServerConnection::ReadRequest() {

	char buffer[MAX_PACKET_SIZE];
	T_Packet *packet = NULL;
	bzero(buffer, MAX_PACKET_SIZE);
	int rcvd = 0;

	alarm(1);

	// Get request message
	if ((rcvd = recvfrom(this->soc, buffer, MAX_PACKET_SIZE ,0,  (struct sockaddr *) &this->socketAddressOut, &this->soutlen)) <0) {  /* read message from client */
		throw ConnectionException("Unable to read client request.",Exception::READ_REQUEST_FAILED);
	}

	alarm(0);

	packet = (T_Packet*) malloc(rcvd * sizeof(char));	
	memcpy(packet, buffer, rcvd);

	return packet;
}

void ServerConnection::SendResponse(T_Packet *value) {

	if (sendto(this->soc, value, value->PACKET_SZ, 0, (struct sockaddr *) &this->socketAddressOut, this->soutlen) < 0 ) {  /* echo message back */
		throw ConnectionException("Unable to send response.",Exception::SEND_RESPONSE_FAILED);
	}

}

void ServerConnection::Close() {
	if (close(this->soc) < 0) { 
		throw ConnectionException("Unable to close connection.",Exception::CLOSE_CONNECTION);
	} 
}

void ClientConnection::SetHostName(std::string value) {
	this->hostname = value;
}

void ClientConnection::CreateSocket() {
	if ((this->soc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { // create socket
		throw ConnectionException("Socket creation failed.",Exception::INVALID_SOCKET);
	}


	memset((char*)&this->socketAddressIn, 0, sizeof(this->socketAddressIn));
	this->socketAddressIn.sin_family = AF_INET; //set protocol family to Internet
	this->socketAddressIn.sin_port = htons(this->port); // set port no.

	if ((hptr =  gethostbyname(this->hostname.c_str())) == NULL){
		throw ConnectionException("Socket creation failed.",Exception::INVALID_SOCKET);
	}		

	memcpy(&this->socketAddressIn.sin_addr,this->hptr->h_addr,this->hptr->h_length);
}

void ClientConnection::SendRequest(T_Packet *value, T_Counter *counter) {
	int sent;
	if ((sent = sendto(this->soc, value, value->PACKET_SZ, 0, 
		(struct sockaddr*) &this->socketAddressIn, 
		sizeof(this->socketAddressIn))) < 0 ) { // send message to server
			throw ConnectionException("Sending request failed.",Exception::SEND_REQUEST_FAILED);
	}
	counter->bytesSent += sent - PACKET_HEADER_SIZE;
}

T_Packet *ClientConnection::ReadResponse(T_Counter *counter, int interval) {
	
	char buffer[MAX_PACKET_SIZE];
	T_Packet *packet = NULL;
	bzero(buffer, MAX_PACKET_SIZE);
	int rcvd;

	alarm(interval);

	// Get request message
	if ((rcvd = recvfrom(this->soc, buffer, MAX_PACKET_SIZE ,0, NULL, NULL)) <0) {  /* read message from client */
		printf("Socket:%d --  %d :: %s\n",this->soc, errno, strerror(errno));
		throw ConnectionException("Unable to read client request.",Exception::READ_REQUEST_FAILED);
	}

	alarm(0);

	counter->bytesReceived += rcvd - PACKET_HEADER_SIZE;

	packet = (T_Packet*) malloc(rcvd * sizeof(char));	
	memcpy(packet, buffer, rcvd);

	return packet;
}

void ClientConnection::Close() {
	if (close(this->soc) < 0) { 
		throw ConnectionException("Unable to close connection.",Exception::CLOSE_CLIENT);
	}
}
