#include "http_tcpServer_linux.hpp"

void	log(const std::string &message){
	std::cout << message <<  std::endl;
}

void exitWithError(const std::string &errorMessage){
	log("ERROR " + errorMessage);
	exit (1);
}

TcpServer::TcpServer(std::string ip_address, int port)
	: m_ip_address(ip_address), m_port(port), m_socket(),
	m_new_socket(), m_incomingMessage(), m_socketAddress(),
	m_socketAddress_len(sizeof(m_socketAddress)),
	m_serverMessage(buildResponse())
{
	m_socketAddress.sin_family = AF_INET;
	m_socketAddress.sin_port = htons(m_port);
	m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address.c_str());
	startServer();
}

TcpServer::~TcpServer(){
	closeServer();
}

int TcpServer::startServer(){
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == -1){
		exitWithError("Cannot creat socket.");
		return 1;
	}
	else if (m_socket >= 0)
		std::cout << "m_socket: " << m_socket << std::endl;
	if (bind(m_socket, (sockaddr *)&m_socketAddress, m_socketAddress_len) < 0){
		exitWithError("Cannot connect socket to address");
	}
	return 0;
}

void TcpServer::startListen(){
	if (listen(m_socket, 20) < 0)
		exitWithError("Socket listen failed");
	std::ostringstream ss;
	ss << "\n*** Listening on ADDRESS: "
		<< inet_ntoa(m_socketAddress.sin_addr) //I dont fully understand this inet_ntoa function.
		<< " PORT: " << ntohs(m_socketAddress.sin_port)
		<< " ***\n\n";
	log(ss.str());

	int bytesReceived;

	while (true){
		log("======= Waiting for a new connection =======\n\n\n");
		acceptConnection(m_new_socket);
		//epoll();
		char buffer[BUFFER_SIZE] = {0};
		bytesReceived = read(m_new_socket, buffer, BUFFER_SIZE);
		if (bytesReceived < 0)
			exitWithError("Failed to read bytes from client socket connection");
		std::ostringstream ss;
		ss << " -------- Received Request from client -------\n\n";
		log(ss.str());

		sendResponse();

		close(m_new_socket);
	}
}

void TcpServer::acceptConnection(int &new_socket){
	new_socket = accept(m_socket, (sockaddr *)&m_socketAddress, &m_socketAddress_len);
	if (new_socket < 0){
		std::ostringstream ss;
		ss << "Server failed to accept incoming from ADDRESS: "
		<< inet_ntoa(m_socketAddress.sin_addr) << "; PORT: "
		<< ntohs(m_socketAddress.sin_port);
		exitWithError(ss.str());
	}
}

std::string TcpServer::buildResponse(){
	std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Serber :) </p></body></html>";
	std::ostringstream ss;
	ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n" << htmlFile;
	return ss.str();
}

void TcpServer::sendResponse(){
	long bytesSent;

	bytesSent = write(m_new_socket, m_serverMessage.c_str(), m_serverMessage.size());
	if (bytesSent == m_serverMessage.size())
		log("------ Server Response sent to client -------\n\n");
	else
		log("Error sending response to client");
}

void TcpServer::closeServer(){
	close(m_socket);
	close(m_new_socket);
	exit (0);
}
