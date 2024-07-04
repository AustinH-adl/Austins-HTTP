#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h> 

#define PORT 8050
#define BUFFER_SIZE 1024

#define REPLY_OK "HTTP/1.1 200 OK\r\n\r\n"
#define REPLY_NOT_FOUND "HTTP/1.1 404 Not Found\r\n\r\n"

enum http_request {
  HTTP_REQUEST_GET,
  HTTP_REQUEST_POST,
  HTTP_REQUEST_PUT,
  HTTP_REQUEST_PATCH,
  HTTP_REQUEST_DELETE,
  HTTP_REQUEST__COUNT,
};

char* processResponse(int id, char* reply);

void *handle_connection(void *vclient_socket) {
	int client_socket = *((int *)vclient_socket); //cast void ptr to int
	int client_socket_len = sizeof(client_socket);
    int bytes_sent;
		
	char response[1024]; //Creating a buffer for response
	char* reply = processResponse(client_socket, response);
	printf("Reply: \n%s\n", reply);

	if(bytes_sent = send(client_socket, reply, strlen(reply), 0) < 0 ) {
		printf("Send failed: %s \n", strerror(errno));
		return NULL;
	}

	printf("Process completed\n");
	printf("-------------\n");
	return NULL;
}

char* processResponse(int id, char* reply) {
	char read_buffer[BUFFER_SIZE];

	if(read(id, read_buffer, BUFFER_SIZE) < 0) {
		printf("Read failed: %s \n", strerror(errno));
    	return REPLY_NOT_FOUND;

 	} else {
    	printf("Request from client: \n %s\n", read_buffer);
  	}

	char *path = strtok(read_buffer, " ");
  	path = strtok(NULL, " "); //Creating a string "path" to the second blank space in the HTTP request where file path is found  

    /*If the request path is a "/" then send a "HTTP 200 ok" response*/
    if (strcmp(path, "/") == 0) {
		return REPLY_OK; 
        
    /*TODO:
    If the request path is a filepath then respond with the corresponding file. 
    Query the filepath in the root dir and find if the requested file exists, if not reply with "not found"*/
	
    /* If the request path is a "/echo/" then respond with the following text. */
    } else if (strncmp(path, "/echo/", 6) == 0) {
		char *echo_string = path + 6;

    	sprintf(reply,
            "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %ld\r\n\r\n%s",
        	strlen(echo_string), echo_string);
		return reply;

    /* If the request path is a "/User-Agent" then respond with the useragent text*/
	} else if(strncmp(path, "/user-agent", 11) == 0) {

		path = strtok(NULL, "\r\n");
		path = strtok(NULL, "\r\n");
		path = strtok(NULL, " ");
		path = strtok(NULL, "\r\n"); //Navigate to the useragent text

		sprintf(reply, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %ld\r\n\r\n%s", strlen(path), path);
		return reply;

    /* If the request path is not a previously defined request then return a "404 not found" response*/
	} else {
		return REPLY_NOT_FOUND;
    }
}

int main() {
    // Disable output buffering
	setbuf(stdout, NULL);
 	setbuf(stderr, NULL);
    
    //Declare server and client address, id and address length for function calls
    int server_fd, client_addr_len, client_id;
    struct sockaddr_in server_addr, client_addr;

    //Attempts to create a server socket
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		perror("SO_REUSEADDR");
        exit(EXIT_FAILURE);
	}

    //Initialize server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    //Bind server address to socket
    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    //Listen for clients
    int connection_backlog = 10; //Max concurrent connections
    if(listen(server_fd, connection_backlog) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    while(1) { //Continues through all clients in backlog
        printf("Waiting for a client to connect...\n");

        if (client_id = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len) < 0) {
            perror("Accept failed");
            continue; //Continue used rather than exit to accept next client
        }
        printf("Client connected\n");
        
        pthread_t thread_id; 
		int *pclient_socket = &client_id;
		pthread_create(&thread_id, NULL, handle_connection, pclient_socket);
    }
    pthread_exit(NULL);
	close(server_fd);
}
