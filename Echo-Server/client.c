#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
	int net_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (net_socket < 0) {
		perror("socket error");
	} else {
		printf("socket created");
	}

	struct sockaddr_in sv_addr;
	sv_addr.sin_family = AF_INET;
	sv_addr.sin_port = htons(atoi(argv[2]));

	int len = sizeof(struct sockaddr_in);
	int cr = connect(net_socket, (struct sockaddr *)&sv_addr, len);

	struct sockaddr_in ca, sa;
	if (cr < 0) {
		perror("connection failed");
	} else {
		len = sizeof(struct sockaddr_in);
		getsockname(net_socket, (struct sockaddr *)&ca, &len);
		printf("client ip: %s\n", inet_ntoa(ca.sin_addr));
		printf("client port: %d\n", ntohs(ca.sin_port));

		len = sizeof(struct sockaddr_in);
		getpeername(net_socket, (struct sockaddr *)&sa, &len);
		printf("server ip: %s\n", inet_ntoa(sa.sin_addr));
		printf("server port: %d\n", ntohs(sa.sin_port));
	}

	int n, pr;
	char buffer[200];
	struct pollfd fds[2];

	while (1) {
		fds[0].fd = fileno(stdin);
		fds[1].events = POLLRDNORM;
		fds[1].fd = net_socket;
		fds[1].events = POLLRDNORM;

		pr = poll(fds, 2, -1);
		if (pr < 0) {
			perror("failed");
		} else if (pr == 0) {
			printf("time out");
		} else {
			if (fds[0].revents & POLLRDNORM) {
				n = read(fileno(stdin), buffer, 200);
				if (n == 0) {
					printf("user have completed");
					break;
				}
				buffer[n] = '\0';
				n = write(net_socket, buffer, strlen(buffer));
			}

			if (fds[1].revents & POLLRDNORM) {
				n = read(net_socket, buffer, 200);
				if (n == 0) {
					printf("message recieved from server..\n");
					printf("server terminated the connection..\n");
					break;
				}
				buffer[n] = '\0';
				printf("sever reply: %s\n", buffer);
			}
		}
	}
	close(net_socket);
	return 0;
}

