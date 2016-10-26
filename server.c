#include <stdio.h>	/* printf, fprintf */
#include <stdlib.h>	/* strtol, exit */
#include <string.h>	/* strlen */
#include <unistd.h>	/* write */
#include <pthread.h>	/* pthread_create */
#include <sys/socket.h>	/* socket, bind, listen, accept */
#include <arpa/inet.h>	/* htons */
#define NAME_LENGTH 21
#define MESSAGE_LENGTH 2001

void *client_handler(void *);

int main(int argc, char* argv[])
{
	int sockfd, port, addrlen, new_socket, *new_sock;
	struct sockaddr_in server, client;

	/* Read cli arguments */
	if (argc < 2)
	{
		fprintf(stderr, "Usage: server port\n");
		exit(1);
	}
	port = strtol(argv[1], NULL, 10);

	/* Create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		fprintf(stderr, "Could not create socket.\n");
		exit(1);
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	/* Bind */
	if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		fprintf(stderr, "Bind failed on port %d.\n", port);
		exit(1);
	}

	/* Listen */
	listen(sockfd, 3);
	printf("Waiting for clients on port %d...\n", port);

	/* Accept and threading*/
	addrlen = sizeof(struct sockaddr_in);
	while ((new_socket = accept(sockfd, (struct sockaddr *)&client, (socklen_t*)&addrlen)))
	{
		pthread_t thread;
		new_sock = malloc(1);
		*new_sock = new_socket;

		if (pthread_create(&thread, NULL, client_handler, (void*) new_sock) < 0)
		{
			fprintf(stderr, "Could not create thread. \n");
			exit(1);
		}

	}

	if (new_socket < 0)
	{
		fprintf(stderr, "Accept failed.\n");
		return 1;
	}

	printf("Exit form main.\n");

	return 0;
}

void *client_handler(void *socket_desc)
{
	int sockfd = *(int*)socket_desc;
	int read_size;
	char *message, name[NAME_LENGTH], client_message[MESSAGE_LENGTH];

	pthread_detach(pthread_self());

	if ((read_size = recv(sockfd, client_message, MESSAGE_LENGTH , 0)) > 0)
	{
		/* Get client's name */
		strncpy(name, client_message, NAME_LENGTH);
		printf("client [%s] log in room []\n", name);
	}

	/* Keep send message to client to test client */
	while (1)
	{
		sleep(10);
		message = "Hello.";
		write(sockfd, message, strlen(message));
		printf("Sending message to [%s]\n", name);
	}
	printf("Exit from thread.\n");

	return 0;
}
