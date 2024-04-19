#include "utils/socket.h"
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define PORT 9999
#define IP ""

struct ClientData {
	int clientSocketFD;
	char name[100];
};

void* messageReceiver(void* arg) {
	struct ClientData* client = (struct ClientData*)arg;
	int clientSocketFD = client->clientSocketFD;
	char* name = client->name;
	char broadcastBuffer[1024];

	while (true) {
		ssize_t charReceived = recv(clientSocketFD, broadcastBuffer, sizeof(broadcastBuffer), 0);
		if (charReceived > 0) {
			broadcastBuffer[charReceived] = 0;
			printf("\r%s\n%s:", broadcastBuffer, name);
			fflush(stdout);
		} else if (charReceived == 0) {
			break;
		}
	}

	close(clientSocketFD);
	free(client);

	return NULL;
}

void messageReceiverThread(struct ClientData* clientData) {
	pthread_t id;
	pthread_create(&id, NULL, messageReceiver, (void*)clientData);
}

int main(int argc, char const* argv[]) {
	int clientSocketFD = createSocket();
	struct sockaddr_in* serverAddress = createAddress(IP, PORT);

	int connectResult = connect(clientSocketFD, (struct sockaddr*)serverAddress, sizeof(*serverAddress));
	if (connectResult != 0) {
		printf("Failed to connect to the server\n");
		exit(1);
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

	struct ClientData* clientData = malloc(sizeof(struct ClientData));
	clientData->clientSocketFD = clientSocketFD;
	strcpy(clientData->name, name);

	char* line = NULL;
	size_t lineSize = 0;
	printf("Hi %s, you can now send messages to the group chat.\n\nType EXIT to leave the chat\n", name);

	messageReceiverThread(clientData);

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
