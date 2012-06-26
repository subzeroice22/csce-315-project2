#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "UnitReversiMain.h"

using namespace std;

//Server

int main() {

	int server_portno;
	int client_length;
	int sock;
	int client_sock;
	
	struct sockaddr_in server_address, client_address;

	//Create server socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sock < 0) {
		perror("socket failed");
		exit(1);
	}	

	//bzero((char*) &server_address, sizeof(server_address));
	memset(&server_address, 0, sizeof(server_address));
	server_portno = 5000;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(server_portno);
	
	//Some magic code to stop unreliable connecting
	int optval=1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR, &optval, sizeof optval);

	//Bind host address
	if(bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		perror("bind failed");
		exit(1);
	}
	
	//Listen for clients (1)
	if(listen(sock, 5) < 0) {
		perror("listen failed");
		exit(1);
	}
	client_length = sizeof(client_address);

	cout << "Server socket:\t" << sock << '\n';
	
	//accept connections	
	client_sock = accept(sock, (struct sockaddr *)&client_address, (socklen_t*)&client_length);
	cout << "Client socket:\t" << client_sock << '\n';
	
	gameStart reversi;
	//reversi.api("START", client_sock);
	reversi.api("START", 0);
	
	//close the client's connection
	close(client_sock);
	
	return 0;
	
}
