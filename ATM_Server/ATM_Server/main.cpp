#include<iostream>
#include<string>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")


void main() {

	//Initialize WinSock
	WSAData winSockData;
	WORD version = MAKEWORD(2,2);
	int winSockOk = WSAStartup(version, &winSockData); //Will return 0 if successful
	if (winSockOk != 0) {
		std::cerr << "ERROR: Could not Initialize WinSocket" << '\n';
		return; 
	}

	//Create a Socket
	SOCKET listeningSock = socket(AF_INET, SOCK_STREAM, 0); //TCP IPV4
	if (listeningSock == INVALID_SOCKET) {
		std::cerr << "ERROR: Could not create a Socket" << '\n';
		return;
	}

	//Bind the IP address and Port to the Socket 
	sockaddr_in address;                         //create an instance of a structure then fill it in.
	address.sin_family = AF_INET;               //AF_INET - Addr family using INET version 4
	address.sin_port = htons(58000);
	address.sin_addr.S_un.S_addr = INADDR_ANY;  //OR you can specify your ip i.e address.sin_addr.s_addr = inet_addr("127.0.0.1")
	
	bind(listeningSock, (sockaddr*)&address, sizeof(address));
	listen(listeningSock, SOMAXCONN);           //Let winSock the sockect is for listening,SOMAXCONN- listen with maximum connection
	
	//Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);
	SOCKET clientSocket = accept(listeningSock, (sockaddr*)&client, &clientSize);

	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "ERROR: Could not create a Client Socket" << '\n';
		return;
	}
	
	char host[NI_MAXHOST];         //Node buffer
	char service[NI_MAXSERV];
	ZeroMemory(host, NI_MAXHOST);  //Fills a block of memory with zeros.
	ZeroMemory(host, NI_MAXSERV);
	if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) { // DNS lookup
		std::cout << "Host Name Connected: " << host << "\nHost Port: " << service << '\n';
		
	}
	else {//Otherwise lets show Host IP
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << "Host IP Connected: " << host << "\nHost Port: " << ntohs(client.sin_port) << '\n';
		
	}
	
	char buf[4096];
	while (true) {
		
		ZeroMemory(buf, 4096);		
		//Wait for client to send data
		int receivedBytes = recv(clientSocket, buf, 4096, 0);
		if (receivedBytes == SOCKET_ERROR) {
			std::cout << "ERROR: Could not recieve data!" << '\n';
			break;
		}
		//If the connection has been gracefully closed
		if (receivedBytes == 0) {
			std::cout << "Host disconnected!" << '\n';
			break;
		}

		
		if (receivedBytes > 0) {
			
			std::cout << "CLIENT: " << std::string(buf, 0, receivedBytes) << '\n';
			
		}
		//Echo message back to client 		
		send(clientSocket, buf, receivedBytes + 1, 0);


	}

	// Close a socket
	closesocket(listeningSock);
	closesocket(clientSocket); 

	//do clean Up
	WSACleanup();
	
	std::cin.get();	
}