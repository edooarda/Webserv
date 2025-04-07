#include <sys/socket.h>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h> //Used to include sockadrr_in std library.
#include <arpa/inet.h> //For inet_ntoa in startListen

#pragma once

class TcpServer{
	public:
		TcpServer(std::string ip_address, int port);
		~TcpServer();

	private:
		std::string m_ip_address;
		int	m_socket;
		int m_port;
		int m_new_socket;
		long m_incomingMessage;
		struct sockaddr_in m_socketAddress; //Standard library
		unsigned int m_socketAddress_len;
		std::string m_serverMessage;

		int	startServer();
		void startListen();
		void closeServer();

};