#include <errno.h>	/* errno */
#include <stdio.h>      /* printf, fprintf */
#include <stdlib.h>     /* strtol, exit */
#include <string.h>	/* strcspn */
#include <unistd.h>	/* STDIN_FILENO */
#include <sys/select.h>	/* select, FD_SET */
#include <sys/socket.h>	/* socket */
#include <arpa/inet.h>  /* htons, inet_addr */
#define NAME_LENGTH 21
#define MESSAGE_LENGTH 2001

int main(int argc, char* argv[])
{
	int sockfd, port, nfds, rc;
	char *host, name[NAME_LENGTH], message[MESSAGE_LENGTH];
	fd_set readfds;
	struct sockaddr_in server;

	/* Read cli arguments */
	if (argc < 3)
	{
		fprintf(stderr, "Usage: client hostip port\n");
		exit(1);
	}
	host = argv[1];
	port = strtol(argv[2], NULL, 10);

	/* Create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) 
	{
		fprintf(stderr, "Could not create socket.\n");
		exit(1);
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(host);
	server.sin_port = htons(port);

	/* Connect to remote server */
	if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		fprintf(stderr, "Connect error.\n");
		exit(1);
	}

	/* User input name */
	printf("[ Welcome! ]\n");
	printf("        Please enter your name: ");
	fgets(name, sizeof(name), stdin);
	name[strcspn(name, "\r\n")] = 0;	/* Remove newline */

	/* Send the name to the server */
	if (send(sockfd, name, strlen(name), 0) < 0)
	{
		fprintf(stderr, "Send failed.\n");
		exit(1);
	}

	/* Read message from local and remote at the same time */
	while (1)
	{
		FD_ZERO(&readfds);
		FD_SET(STDIN_FILENO, &readfds);
		FD_SET(sockfd, &readfds);
		nfds = sockfd > STDIN_FILENO ? sockfd + 1 : STDIN_FILENO + 1;
		rc = select(nfds, &readfds, NULL, NULL, NULL);
		if (rc < 0)
		{
			if (errno != EINTR)
			{
				fprintf(stderr, "Select failed.\n");
				exit(1);
			}
		}
		if (FD_ISSET(STDIN_FILENO, &readfds))
		{
			fgets(message, sizeof(message), stdin);
			message[strcspn(message, "\r\n")] = 0;	/* Remove newline */
			if (strcmp(message, "Bye") == 0)
			{
				break;
			}
			if (send(sockfd, message, MESSAGE_LENGTH, 0) < 0)
			{
				fprintf(stderr, "Send failed.\n");
				exit(1);
			}
		}
		else if (FD_ISSET(sockfd, &readfds))
		{
			if (recv(sockfd, message, MESSAGE_LENGTH, 0) < 0)
			{
				fprintf(stderr, "Recv failed.\n");
				exit(1);
			}
			printf("%s\n", message);
		}
	}

	return 0;
}
