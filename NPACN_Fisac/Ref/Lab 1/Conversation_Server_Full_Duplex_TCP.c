#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/wait.h>
#include<sys/select.h>
#define MAXSIZE 90

main()
{
	int sockfd,newsockfd,retval,i;
	socklen_t actuallen;
	int recedbytes,sentbytes;
	struct sockaddr_in serveraddr,clientaddr;
	char buff[MAXSIZE];
	
	int a=0;
	sockfd=socket(AF_INET,SOCK_STREAM,0);

	if(sockfd==-1)
	{
	printf("\nSocket creation error");
	}

	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(3388);
	serveraddr.sin_addr.s_addr=htons(INADDR_ANY);
	retval=bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	if(retval==-1)
	{
		printf("Binding error");
		close(sockfd);
	}

	retval=listen(sockfd,1);
	if(retval==-1)
	{
	close(sockfd);
	}
	actuallen=sizeof(clientaddr);
	newsockfd=accept(sockfd,(struct sockaddr*)&clientaddr,&actuallen);
	if(newsockfd==-1)
	{
		close(sockfd);
	}
	
	int pipefd[2];
	pipe(pipefd);
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
		close(sockfd);
		return 1;
	}
	if (pid == 0) { // Child process: handle receiving
		close(pipefd[1]); // Close write end
		close(sockfd); 
		fd_set readfds;
		int maxfd = newsockfd > pipefd[0] ? newsockfd : pipefd[0];
		while (1) {
			FD_ZERO(&readfds);
			FD_SET(newsockfd, &readfds);
			FD_SET(pipefd[0], &readfds);
			select(maxfd + 1, &readfds, NULL, NULL, NULL);
			if (FD_ISSET(pipefd[0], &readfds)) {
				break; // Stop signal from parent
			}
			if (FD_ISSET(newsockfd, &readfds)) {
				memset(buff, '\0', sizeof(buff));
				recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
				if (recedbytes == -1) {
					break;
				}
				if (buff[0] == 's' && buff[1] == 't' && buff[2] == 'o' && buff[3] == 'p') {
					write(pipefd[1], "stop", 4); // Signal parent to stop
					break;
				}
				printf("%s \n", buff);
			}
		}
		close(newsockfd);
		close(pipefd[0]);
		exit(0);
	} else { // Parent process: handle sending
		close(pipefd[0]); 
		close(sockfd);
		while (1) {
			memset(buff, '\0', sizeof(buff));
			scanf("%s", buff);
			buff[strlen(buff)] = '\0';
			int s = strlen(buff) * sizeof(char);
			sentbytes = send(newsockfd, buff, s, 0);
			if (sentbytes == -1) {
				break;
			}
			if (buff[0] == 's' && buff[1] == 't' && buff[2] == 'o' && buff[3] == 'p') {
				write(pipefd[1], "stop", 4); // Signal child to stop
				break;
			}
		}
		close(newsockfd);
		close(pipefd[1]);
		wait(NULL); // Wait for child
	}
	close(sockfd);
}

