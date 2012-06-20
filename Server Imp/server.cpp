#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "gameStart.h"


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

	//bzero((char*) &server_address, sizeof(server_address));
	memset(&server_address, 0, sizeof(server_address));
	server_portno = 5000;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(server_portno);

	//Bind host address
	bind(sock, (struct sockaddr *) &server_address, sizeof(server_address));

	//Listen for clients (1)
	listen(sock, 1);
	client_length = sizeof(client_address);

	//accept connections
	
	client_sock = accept(sock, (struct sockaddr *)&client_address, (socklen_t*)&client_length);
		
	//send client connection message, check if send() was successful
	int send_status = send(client_sock, "null", 0, 0);	
	
	cout << "Server socket:\t" << sock << '\n';
	cout << "Send status:\t" << send_status << '\n';
	cout << "Client socket:\t" << client_sock << '\n';
	
	gameStart reversi;
	reversi.api("START", client_sock);
	
	return 0;
	
}
