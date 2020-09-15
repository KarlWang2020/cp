#include<stdio.h>
#include<errno.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<signal.h>
#include<sys/wait.h>
#include<stdlib.h>

const int MAX_DATA_SIZE = 100;

void* Get_inaddr(struct sockaddr* sa) { // to get input socket address
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}
int writen(int sockfd, char* buf) {
	int Nbytes;  // Nbytes marks total bytes here
	while ((Nbytes = send(sockfd, buf, MAX_DATA_SIZE - 1, 0)) == -1 && errno ==
		EINTR) {		continue; }
return Nbytes;  
}

int readline(int sockfd, char* recvbuf) { // readline from the socket
	int Nbytes;
	while (errno== EINTR&&(Nbytes = recv(sockfd, recvbuf, MAX_DATA_SIZE - 1, 0)) == -1  ) {
		
	}
	return Nbytes;
}
int server_lookup_connect(char* host, char* server_port) {   //this function looks up to connect
	int status;
	int sock_fd;
	struct addrinfo hints, * server_info, * p;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	if ((status = getaddrinfo(host, server_port, &hints, &server_info)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));    
		return 2;
	}
	for (p = server_info; p != NULL; p = p->ai_next) { //loop through link list
		sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock_fd == -1) { //socket creation failed
			perror("client: socket");
			continue;
		}
		if (connect(sock_fd, p->ai_addr, p->ai_addrlen) == -1) { //connection failed
			close(sock_fd);
			perror("client: connect"); //client connection
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "client:  connect failed\n");
		return 2;
	}
	printf("client: connected to %s:%s\n", host, server_port);
	freeaddrinfo(server_info);
	return sock_fd;
}
int main(int argc, char* argv[]) {  // Main function,and argv[1]: IPAdr and argv[2]: Port
	int sock_fd;
	int Nbytes_send;
	int Nbytes_recv;
	char buf[MAX_DATA_SIZE], recvbuf[MAX_DATA_SIZE];
	char* host, * server_port;
	if (argc == 3) {
		host = argv[1];
		server_port = argv[2];
		printf("Echo Service starting \n");
		printf("client: client started\n");
	}
	else {
		fprintf(stderr, "usage: echo IPAdr Port\n");
		exit(1);
	}
	sock_fd = server_lookup_connect(host, server_port);
	// read from the stdin,fget can limit input size
	while (fgets(buf, MAX_DATA_SIZE, stdin)) {
		Nbytes_send = writen(sock_fd, buf);
		if (Nbytes_send == -1) { //writen and handle error 
			perror("send");
			exit(1);
		}
		Nbytes_recv = readline(sock_fd, recvbuf); //numbytes received
		if (Nbytes_recv == -1) {
			perror("recv");
			exit(1);
		}
		//successful write and receive echo
		printf("%s\n", recvbuf);
		
		if (strcmp(buf, recvbuf) != 0) { //if buffer is not same
			if (strncmp(buf, recvbuf, Nbytes_recv) == 0) { // if recv string is a substring of send string
				printf(" !recv buffer out of space.\n");
			}
			else { //send  error of mismatching
				perror(" !client: received string did not match");
				exit(1);
			}
		}
	}
	printf("socket closed \n"); 
	close(sock_fd);
	return 0;
}
