#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void)
{
	int net_socket = socket(AF_INET, SOCK_STREAM, 0); // 0 can be written as -> IPPROTO_TCP
	// 0 signifies the default communication protocol, which is TCP

	if (net_socket < -1)
	{
		perror("socket creation failed");
	}
	else
	{
		perror("socket created succesfully");
	}

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(33456); // specified port number
	serv_addr.sin_addr.s_addr = inet_addr("0.0.0.0");

	// creating the bind function
	/*
	 * bind() function: if bind function is not created then random port number will be assigned.
	 * so bind function is not neccesary for server code as the server can still generate random port number
	 * and the client side can get connected to the port
	 */

	int len = sizeof(serv_addr);
	// bind()
	/*
	int br = bind(net_socket, (struct sockaddr *)&serv_addr, len);

	// bind() function on success return 0 and on failure returns -1
	if (br < 0) {
		perror("bind failed");
	} else {
		perror("bind successful");
	} */

	// creating the listen function
	int lr = listen(net_socket, 10);
	if (lr < 0)
	{
		perror("listen failed");
	}
	else
	{
		len = sizeof(struct sockaddr_in);
		getsockname(net_socket, (struct sockaddr *)&serv_addr, &len);

		printf("sevrer ip: %s\n", inet_ntoa(serv_addr.sin_addr));
		printf("server is listening on port number: %d\n", ntohs(serv_addr.sin_port));
	}

	int fd, n;
	struct sockaddr_in ca, sa;
	pid_t pid;
	char buffer[200];

	while (1)
	{
		len = sizeof(struct sockaddr_in);
		fd = accept(net_socket, (struct sockaddr *)&ca, &len);

		if (fd < 0)
		{
			perror("accept failed");
		}
		else
		{
			len = sizeof(struct sockaddr_in);
			getsockname(fd, (struct sockaddr *)&sa, &len);

			printf("local ip for connection: %s\n", inet_ntoa(sa.sin_addr));
			printf("local port for connection: %d\n", ntohs(sa.sin_port));

			len = sizeof(struct sockaddr_in);
			getpeername(fd, (struct sockaddr *)&ca, &len);

			printf("client ip for connection: %s\n", inet_ntoa(ca.sin_addr));
			printf("client port for connection: %d\n", ntohs(ca.sin_port));

			pid = fork();
			if (pid < 0)
				perror("fork failed");
			else if (pid == 0)
			{
				close(net_socket);
				while (1)
				{
					n = read(fd, buffer, 200);
					if (n == 0)
					{
						printf("client closed communication");
						break;
					}

					buffer[n] = '\0';
					write(fd, buffer, strlen(buffer));
				}
				close(fd);
				exit(0);
			}
			else
			{
				close(fd);
			}
		}
	}
	return 0;
}
