#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>

// Client

int server_sock;
int room_sock;
int room_portno;
int server_connection;
int rec;
int command_send_status;	
int room_connection;	

char buffer[100];
int server_portno = 5000;

struct sockaddr_in server_address;
struct sockaddr_in room_address;

pthread_t threads[2];

void* incoming(void* arg) {
	while (1) {
		char buff[100];
		if(recv(server_sock, buff, 100, 0) > 0) {
			printf(">>> %s\n", buff);
		}
	}
}

void* outgoing(void* arg) {
	while(1) {	
		char buff[100];
		gets(buff);
		send(room_sock, buffer, 100, 0);
	}
}

int main() {

	//Variable initializations

	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	printf("Server sock: %d\n", server_sock);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_address.sin_port = htons(server_portno);

	//Connect to the server
	server_connection = connect(server_sock, (struct sockaddr *)&server_address, sizeof(server_address));
	printf("Server Connection Status:\t%d\n",server_connection);
	
	//Receive connection message
	rec = recv(server_sock, buffer, 100, 0); 
	
	buffer[rec] = '\0';
	printf("Message from server:\t%s\n",buffer);
	buffer[0] = '\0';

	//Get input for command and send the command
	gets(buffer);
	command_send_status = send(server_sock, buffer, 100, 0);

	//Receive port number
	rec = recv(server_sock, buffer, 100, 0);
	room_portno = atoi(buffer);
	printf("Port number received: %d\n", room_portno);

	room_sock = socket(AF_INET, SOCK_STREAM, 0);
	printf("Room sock: %d\n", room_sock);

	room_address.sin_family = AF_INET;
	room_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	room_address.sin_port = htons(room_portno);

	room_connection = connect(room_sock, (struct sockaddr *)&room_address, sizeof(room_address));
	printf("Room connection status: %d\n", room_connection);
	rec = recv(room_sock, buffer, 100, 0);
	printf("Rec status: %d\n", rec);
	printf("Message from room: %s\n", buffer);

	pthread_create(&threads[0], NULL, incoming, NULL);
	//pthread_create(&threads[1], NULL, outgoing, NULL);

	for(;;) {
		buffer[0] = '\0';
		gets(buffer);

		int send_status = send(server_sock, buffer, 100, 0);
		printf("Send status: %d\tMessage sent: %s\n", send_status, buffer);
	}

}
