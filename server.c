#include "utils/socket.h"
#include <stdbool.h>
#include <unistd.h>

#define PORT 9999
#define IP ""

int main(int argc, char const* argv[]) {
	int serverSocketFD = createSocket();
	const struct sockaddr_in* serverAddress = createAddress(IP, PORT);

	int bindResult = bind(serverSocketFD, (struct sockaddr*)serverAddress, sizeof(*serverAddress));
	if (bindResult == 0) {
		printf("Socket bound successfully\n");
	}

	int listenResult = listen(serverSocketFD, 10);
	if (listenResult == 0) {
		printf("Server is listening on %d\n", PORT);
	}

	struct sockaddr_in clientAddress;
	socklen_t clientAddressSize = (socklen_t)sizeof(clientAddress);
	int clientFD = accept(serverSocketFD, (struct sockaddr*)&clientAddress, &clientAddressSize);

	char clientName[100];
	while (true) {
		ssize_t nameLength = recv(clientFD, clientName, sizeof(clientName), 0);
		printf("%s connected successfully\n", clientName);
		if (nameLength > 0)
			break;
	}

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	while (true) {
		ssize_t receivedChar = recv(clientFD, buffer, sizeof(buffer), 0);
		if (receivedChar > 0) {
			buffer[receivedChar] = 0;
			printf("%s: %s", clientName, buffer);
		}
	}

	close(clientFD);
	shutdown(serverSocketFD, SHUT_RDWR);

	return 0;
}
