#include "utils/socket.h"

int main(int argc, char const* argv[]) {
	int serverSocketFD = createSocket();
	struct sockaddr_in* serverAddress = createAddress("", 9999);

	int bindResult = bind(serverSocketFD, serverAddress, sizeof(*serverAddress));
	if (bindResult == 0) {
		printf("Socket bound successfully");
	}

	int listenResult = listen(serverSocketFD, 10);
	if (listenResult == 0) {
		printf("Accepted a client");
	}

	struct sockaddr_in* clientAddress;
	int clientAddressSize = sizeof(clientAddress);

	int clientFD = accept(serverSocketFD, clientAddress, &clientAddressSize);

	char* message;
	message = "Server message";
	send(clientFD, message, sizeof(message), 0);

	return 0;
}
