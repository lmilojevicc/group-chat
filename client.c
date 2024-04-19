#include "utils/socket.h"
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define PORT 9999
#define IP ""

int main(int argc, char const* argv[]) {
	int clientSocketFD = createSocket();
	struct sockaddr_in* serverAddress = createAddress(IP, PORT);

	int connectResult = connect(clientSocketFD, (struct sockaddr*)serverAddress, sizeof(*serverAddress));
	if (connectResult == 0) {
		printf("Successfully connected to the server\n");
	}

	char name[100];
	printf("Please provide us with your name:");
	fgets(name, sizeof(name), stdin);
	int newLineIndex = strcspn(name, "\n");
	name[newLineIndex] = 0;

	if (name[0] == '\0') {
		printf("Okay you will stay anonymous\n");
		srand(time(NULL));
		int randomNum = rand() % 1000;
		sprintf(name, "Anonymous#%d", randomNum);
	}

	send(clientSocketFD, name, strlen(name), 0);

	char* line = NULL;
	size_t lineSize = 0;
	printf("Hi %s, you can now send messages to the group chat.\n\nType EXIT to leave the chat\n", name);

	while (true) {
		printf("%s:", name);
		ssize_t charCount = getline(&line, &lineSize, stdin);

		if (charCount > 0 && strcmp(line, "EXIT\n") == 0) {
			break;
		}

		ssize_t charSentCount = send(clientSocketFD, line, charCount, 0);
	}

	close(clientSocketFD);

	return 0;
}
