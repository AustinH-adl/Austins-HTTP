#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h> 

#define BUFFER_SIZE 1024
#define PORT 8080

#define REPLY_OK "HTTP/1.1 200 OK\r\n\r\nOK"
#define REPLY_NOT_FOUND "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n404NotFound"


/* Process any HTTP Get requests 
Todo: Add a function to process Post responses if needed*/
char* processGet(char response[], char request[BUFFER_SIZE]) {
	char* path = strtok(request, " ");
	path = strtok(NULL, " ");

	/*Compare the path to / and send "HTTP ok"*/
	if (strcmp(path, "/") == 0) {
		return REPLY_OK;

	/*Compare the path to "Echo" and send the string following echo*/
	} else if (strncmp(path, "/echo/", 6) == 0) {
		char *echo_string = path + 6;
		sprintf(response,
			"HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %ld\r\n\r\n%s",
			strlen(echo_string), echo_string);
		return response;
	
	/* Compare the path to "/user-agent" and sends the request's user agent header*/
	} else if(strncmp(path, "/user-agent", 11) == 0) {
		path = strtok(NULL, "\r\n");
		path = strtok(NULL, "\r\n");
		path = strtok(NULL, " ");
		path = strtok(NULL, "\r\n");
		sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %ld\r\n\r\n%s", strlen(path), path);
		return response;

	/*If none of the defined methods occur then send a not found*/
	} else {
		return REPLY_NOT_FOUND;
	}
	return REPLY_NOT_FOUND;
}

char* processResponse(int id, char response[]) {
	char buffer[BUFFER_SIZE];
	ssize_t bytesread;
	/*Read the HTTP request and save to buffer*/
	if (bytesread = read(id, buffer, BUFFER_SIZE) <= 0) {
		printf("Read failed: %s \n", strerror(errno));
    	return REPLY_NOT_FOUND;
 	} else {
    	printf("Request from client:\n%s\n", buffer);
  	}

	if (strncmp(buffer, "GET", 3) == 0 ) {
		return processGet(response, buffer);
	} else {
		return REPLY_NOT_FOUND;
	}
}

void *handle_connection(void *vclient_socket) {
	int client_socket = *((int *)vclient_socket); //cast void ptr to int in order to be handled on a thread
	
	//Initialize and pass buffer to ProcessResponse
	char response[BUFFER_SIZE];
	char* reply = processResponse(client_socket, response);

	printf("Reply: \n%s\n", reply);
	
	/*Send the processed response*/
	int bytes_sent;
	if(bytes_sent = send(client_socket, reply, strlen(reply), 0) < 0 ) {
		printf("Send failed: %s \n", strerror(errno));
		return NULL;
	}
	
	close(client_socket);
	printf("Process complete\n");
	printf("-------------\n");
	return NULL;
}

int main() {
	// Disable output buffering
	setbuf(stdout, NULL);
 	setbuf(stderr, NULL);

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	printf("Logs from your program will appear here!\n");
	
	int server_fd, client_addr_len;
	struct sockaddr_in client_addr;
	
	/*Initialize socket*/
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		printf("Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}
	
	//Allow reusing the port for faster testing and reinitialization of the server
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		printf("SO_REUSEADDR failed: %s \n", strerror(errno));
		return 1;
	}
	
	//Set the server address settings most notably the port
	struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
									 .sin_port = htons(PORT),
									 .sin_addr = { htonl(INADDR_ANY) },
									};
	
	//Bind socket to the server address
	if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("Bind failed: %s \n", strerror(errno));
		return 1;
	}

	int connection_backlog = 10; //Backlog for concurrent connections
	//Listen for incomming requests 
	if (listen(server_fd, connection_backlog) < 0 )	 {
		printf("Listen failed: %s \n", strerror(errno));
		return 1;
	}

	while(1) { //Work through backlog		
		//Accept any incomming connection 
		int client_id = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
		if (client_id < 0) {
			printf("Client accept failed: %s \n", strerror(errno));
			continue;
		}
		printf("Client connected\n");

		/*Create a thread_id for creating concurrent connections*/
		pthread_t thread_id; 
		int *pclient_socket = &client_id; //Casting client_id to a ptr so handle_connection can be used
		pthread_create(&thread_id, NULL, handle_connection, pclient_socket);
		pthread_detach(thread_id);
	}
	//Catch any still running threads and close them then close the server
	pthread_exit(NULL);
	close(server_fd);

	return 0;
}
