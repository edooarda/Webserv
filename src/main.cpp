// #include <iostream>

// // int main (int argc, char **argv)
// // {
// // 	if (argc > 2)
// // 	{
// // 		std::cout << "Wrong Amount of Arguments" << std::endl;
// // 		return 1;
// // 	}
// // 	else
// // 	{
// // 		if(argc == 2)
// // 		{
// // 			std::cout << argv[1] << std::endl;
// // 			//config file should be used as argv[1]
// // 		}
// // 		else
// // 		{
// // 			std::cout << "Default"<< std::endl;
// // 			// should use a default config file
// // 		}
// // 	}
// // }

// /*
// 1. Create a socket with function socket() (IPv4 and TCP).
// 2. Bind the socket with a port 8080 with bind().
// 3. You make the socket listen() --> this will make it a "server socket".
// 4. 

// */

// /*
// 	struct epoll_event something[1000];
// 	epoll_wait()
// 	epoll_event something[0] = data.fd
// 	epoll_event something[1] = data.fd
// */

// /*handle client(int fd) {
// 	//what you are sending needs to be in the same protocol
// 	//<html><h1>Hello World!!</h1></html>
// 	send(client_fd, what)
// }
// */

// int main() {
// 	//create the server socket
// 	//create the 1 (only one!) epoll();
// 	while(true) {
// 		//number of ready events = epoll_wait() --> return the number of "ready fd's".
// 		//for loop that loops through the "ready fd's" --> epoll
// 		//if (data.fd == server.fd)
// 		// accept the client with accept();
// 		// add it to the epoll instance with epoll_ctl(client_fd)
// 		//else
// 		// int client_fd = event.data.fd
// 		//	handle_client(client_fd)
// 	}
// }


#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#define PORT 8080
#define MAX_EVENTS 10

void handle_client(int client_fd) {
	char buffer[1024];
	read(client_fd, buffer, sizeof(buffer));
	std::cout << buffer << std::endl;
    const char response[] = "HTTP/1.1 200 Ok\r\nContent-Type: text/html\r\n\r\n<html><h1>HELLO WORLD!</h1></html>";
    send(client_fd, response, sizeof(response), 0);
    close(client_fd);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "Socket created succesfully "  << server_fd << std::endl;
    if (server_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind error " << strerror(errno) << std::endl;
        return 1;
    }
	std::cout << "Bind on port " << PORT << std::endl;

    if (listen(server_fd, SOMAXCONN) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

    int epoll_fd = epoll_create(MAX_EVENTS);
    if (epoll_fd == -1) {
        std::cerr << "Epoll create failed" << std::endl;
        return 1;
    }

    epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);

    epoll_event events[MAX_EVENTS];

    while (true) {
        int num_ready = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < num_ready; ++i) {
            if (events[i].data.fd == server_fd) {
                sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
                if (client_fd >= 0) {
                    epoll_event client_event{};
                    client_event.events = EPOLLIN;
                    client_event.data.fd = client_fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
                }
            } else {
                handle_client(events[i].data.fd);
            }
        }
    }

    close(server_fd);
    close(epoll_fd);
    return 0;
}