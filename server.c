#include "utils/socket.h"
#include <stdbool.h>
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

	printf("Client connected successfully\n");

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	recv(clientFD, buffer, sizeof(buffer), 0);
	printf("Message from the client: %s\n", buffer);

	return 0;
}
