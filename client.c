#include "utils/socket.h"
#define PORT 9999
#define IP ""

int main(int argc, char const* argv[]) {
	int clientSocketFD = createSocket();
	struct sockaddr_in* serverAddress = createAddress(IP, PORT);

	int connectResult = connect(clientSocketFD, (struct sockaddr*)serverAddress, sizeof(*serverAddress));
	if (connectResult == 0) {
		printf("Successfully connected to the server\n");
	}

	char* message;

	message = "Hello server\n";
	send(clientSocketFD, message, strlen(message), 0);
	printf("Message sent to server\n");

	return 0;
}
