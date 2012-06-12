#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

//Server

//Room struct with room name, port num, and user list
typedef struct {
	char* room_name;
	int port_num;
	int users[10];
} room;

//Client struct to hold the client sock and which room client is in
typedef struct {
	int client_sock;
	room chat_room;
} client;

int num_of_clients = 0;
room room_array[10];
pthread_t threads[100];
int foo = 0;

int addRoom(char* room_name) {
	int spot = 0;	
	while( room_array[spot].room_name != NULL ) {
		if(spot != 10) {
			spot++;
		} 
		else { 
			printf("Too many rooms\n");
			return -1;
		}	
	}
	room_array[spot].room_name = room_name;
	room_array[spot].port_num = (spot + 5001);
	return spot + 5001;
}

room lookupRoom(char* room_name) {
	int spot = 0;
	while( strcmp(room_array[spot].room_name, room_name) != 0 ) {
		if(spot != 10) {
			spot++;
		}
		else {
			printf("Could not find room\n");
			printf("%s\t%s\n", room_name, room_array[0].room_name);
			//return NULL;
			break;
		}
	}
	return room_array[spot];
}

void* chat(void* client_obj) {
	char* buffer;
	int client_sock = ((client*)client_obj)->client_sock;
	printf("Chat client sock: %d\n", client_sock);
	int send_room_connect;	
	send_room_connect = send(client_sock, "Connected to room", 100, 0);
	printf("Room connection msg sent: %d\n", send_room_connect);
	room chat_room = ((client*)client_obj)->chat_room;
	while( 1 ) {
		int foo = recv(client_sock, buffer, 100, 0);
		if(foo > 0) {
			for(int i = 0; i < 10; i++) {
				send(chat_room.users[i], buffer, 100, 0);
			}
		}
	}
}

void* connect_client(void* client_obj) {
	char buffer[100];
	//char* buff = (char*)malloc(100);
	int client_sock = ((client*)client_obj)->client_sock;
	if( client_sock < 0 ) {
		printf("Error on client connection\n");
		return NULL;
	} 
	int rec = recv(client_sock, buffer, 100, 0);
	int room_portno;
	
	buffer[rec-1] = '\0';

	char room_port_str[100];
	
	char *command;
	char *room;
	char *space = " ";

	command = strtok(buffer, space);
	room = strtok(NULL, space);

	if( strcmp(command, "JOIN") == 0 ) {
		// call join function
		printf("JOIN function gets called on room: %s\n",room);
		room_portno = lookupRoom(room).port_num;
		sprintf(room_port_str, "%d", room_portno);
		int port_num_sent = send(client_sock, room_port_str, 100, 0);
		chat(client_obj);
	} else {
		if( strcmp(command, "CREATE") == 0 ) {
			printf("CREATE function gets called on room: %s\n",room);
			
			int client_length;
			int room_sock;
			int client_room;
			int port_num_sent;			

			struct sockaddr_in server_address, client_address;

			//Create room socket
			room_sock = socket(AF_INET, SOCK_STREAM, 0);
			printf("Room socket:\t%d\n", room_sock);	
			
			//bzero((char*) &server_address, sizeof(server_address));
			room_portno = addRoom(room);
			printf("Room portno: %d\n", room_portno);			

			//send port number
			sprintf(room_port_str, "%d", room_portno);
			port_num_sent = send(client_sock, room_port_str, 100, 0);
			printf("Port num sent status: %d\n", port_num_sent);			

			server_address.sin_family = AF_INET;
			server_address.sin_addr.s_addr = INADDR_ANY;
			server_address.sin_port = htons(room_portno);

			//Bind host address
			bind(room_sock, (struct sockaddr*) &server_address, sizeof(server_address));

			//Listen for clients (10)
			listen(room_sock, 10);
			client_length = sizeof(client_address);
			
			while(1) {
				int new_client = accept(client_sock, (struct sockaddr *)&client_address, 
				(socklen_t*)&client_length);
				client cli = {new_client, lookupRoom(room)};
				if(new_client == 0) {
					foo++;
					int a = pthread_create(&threads[foo], NULL, chat, &cli);
					printf("new_client: %d\n", new_client);
				}
			}
				
			printf("Reach end of createRoom(room)\n");
		} else {
			if( strcmp(command, "DELETE") == 0 ) {
				printf("DELETE function gets called on room: %s\n",room);
			}	else { printf("ERROR, invalid command: %s\n", command); }

		}


	}

}


int main() {

	int server_portno;
	int client_length;
	int sock;
	int client_sock;

	//pthread_t threads[100];

	struct sockaddr_in server_address, client_address;

	//Create server socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	printf("Server socket:\t%d\n", sock);

	bzero((char*) &server_address, sizeof(server_address));
	server_portno = 5000;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(server_portno);

	//Bind host address
	bind(sock, (struct sockaddr *) &server_address, sizeof(server_address));

	//Listen for clients (10)
	listen(sock, 10);
	client_length = sizeof(client_address);

	//accept connections
	for(;;) {
		client_sock = accept(sock, (struct sockaddr *)&client_address, (socklen_t*)&client_length);
		
		//send client connection message, check if send() was successful
		int send_status = send(client_sock, "Connected", 9, 0);	
		printf("Send status:\t%d\n", send_status);		
	
		client cli = {client_sock};
		foo++;
		int a = pthread_create(&threads[foo], NULL, connect_client, &cli);
		num_of_clients++;
		printf("New socket:\t%d\n",client_sock);
	}
}
