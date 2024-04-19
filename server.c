#include "utils/socket.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define PORT 9999
#define IP ""

struct clientSocket {
	int clientSocketFD;
	struct sockaddr_in clientAddress;
	char clientName[100];
	int error;
	bool connected;
};

struct clientSocket acceptClient(int serverSocketFD) {

	struct sockaddr_in clientAddress;
	socklen_t clientAddressSize = (socklen_t)sizeof(clientAddress);
	int clientFD = accept(serverSocketFD, (struct sockaddr*)&clientAddress, &clientAddressSize);

	char name[100];
	ssize_t nameLength = recv(clientFD, name, sizeof(name), 0);
	name[nameLength] = '\0';
	printf("%s entered the chat\n", name);

	struct clientSocket client;
	client.clientSocketFD = clientFD;
	client.clientAddress = clientAddress;
	strcpy(client.clientName, name);
	client.error = 0;
	client.connected = true;

	return client;
}

int main(int argc, char const* argv[]) {
	int serverSocketFD = createSocket();
	const struct sockaddr_in* serverAddress = createAddress(IP, PORT);

	int bindResult = bind(serverSocketFD, (struct sockaddr*)serverAddress, sizeof(*serverAddress));

	if (bindResult == 0) {
		printf("Socket bound successfully\n");
	} else {
		printf("Socket binding failed\n");
		exit(1);
	}

	int listenResult = listen(serverSocketFD, 10);
	if (listenResult == 0) {
		printf("Server is listening on %d\n", PORT);
	}

	struct clientSocket client = acceptClient(serverSocketFD);

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	while (true) {
		ssize_t receivedChar = recv(client.clientSocketFD, buffer, sizeof(buffer), 0);

		if (receivedChar > 0) {
			buffer[receivedChar] = 0;
			printf("%s: %s", client.clientName, buffer);
		} else {
			printf("%s has left the chat\n", client.clientName);
			client.connected = false;
			break;
		}
	}

	close(client.clientSocketFD);
	shutdown(serverSocketFD, SHUT_RDWR);

	return 0;
}
