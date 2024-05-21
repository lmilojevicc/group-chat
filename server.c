#include "utils/socket.h"
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define PORT 9999
#define IP ""
#define MAX_CLIENTS 10
struct clientSocket {
	int clientSocketFD;
	struct sockaddr_in clientAddress;
	char clientName[100];
	int error;
	bool connected;
};

struct clientSocket acceptedClients[MAX_CLIENTS];
int currentClients = 0;
pthread_mutex_t logMutex = PTHREAD_MUTEX_INITIALIZER;

void logData(char* message) {
	pthread_mutex_lock(&logMutex);

	FILE* log = fopen("server-log.txt", "a");
	if (log == NULL) {
		printf("Couldn't open a file");
	}

	time_t rawTime;
	struct tm* timeInfo;
	time(&rawTime);
	timeInfo = localtime(&rawTime);

	char dateString[64];
	strftime(dateString, sizeof(dateString), "%Y-%m-%d %H:%M:%S", timeInfo);

	fprintf(log, "%s - %s\n", dateString, message);
	fclose(log);

	pthread_mutex_unlock(&logMutex);
}

void broadcastMessages(char* buffer, int clientSocketFD) {
	for (int i = 0; i < currentClients; i++) {
		if (acceptedClients[i].clientSocketFD != clientSocketFD) {
			send(acceptedClients[i].clientSocketFD, buffer, strlen(buffer), 0);
		}
	}
}

void handleTerminationSignal(int signal) {
	char* message = "Server is shutting down. Goodbye!";
	broadcastMessages(message, -1);
	logData("Server is shutting down");
	exit(0);
}

struct clientSocket* createClientSocket(int serverSocketFD) {
	struct sockaddr_in clientAddress;
	socklen_t clientAddressSize = sizeof(clientAddress);
	int clientFD = accept(serverSocketFD, (struct sockaddr*)&clientAddress, &clientAddressSize);

	char name[100];
	ssize_t nameLength = recv(clientFD, name, sizeof(name), 0);
	name[nameLength] = '\0';

	struct clientSocket* client = malloc(sizeof(struct clientSocket));
	client->clientSocketFD = clientFD;
	client->clientAddress = clientAddress;
	strcpy(client->clientName, name);
	client->connected = clientFD > 0;
	if (!client->connected) {
		client->error = client->clientSocketFD;
	}

	char logMessage[100];
	snprintf(logMessage, sizeof(logMessage), "%s joined the chat", name);
	printf("%s\n", logMessage);
	logData(logMessage);

	char broadcastMsg[100];
	snprintf(broadcastMsg, sizeof(broadcastMsg), "%s joined the chat", name);
	broadcastMessages(broadcastMsg, client->clientSocketFD);

	return client;
}

void* handleClientMessages(void* arg) {
	struct clientSocket* client = (struct clientSocket*)arg;
	char buffer[1024];

	while (true) {
		memset(buffer, 0, sizeof(buffer));
		ssize_t receivedChar = recv(client->clientSocketFD, buffer, sizeof(buffer), 0);

		if (receivedChar > 0) {
			buffer[receivedChar] = 0;
			printf("%s:%s", client->clientName, buffer);

			int newLineIndex = strcspn(buffer, "\n");
			buffer[newLineIndex] = 0;

			char* broadcastMessage = malloc(sizeof(buffer) + sizeof(client->clientName));
			sprintf(broadcastMessage, "%s:%s", client->clientName, buffer);
			broadcastMessages(broadcastMessage, client->clientSocketFD);
		} else {
			char logMessage[100];
			sprintf(logMessage, "%s has left the chat\0", client->clientName);
			printf("%s\n", logMessage);
			logData(logMessage);

			broadcastMessages(logMessage, client->clientSocketFD);

			client->connected = false;
			break;
		}
	}

	close(client->clientSocketFD);
	free(client);

	return NULL;
}

void acceptClientConnections(int serverSocketFD) {
	while (true) {
		struct clientSocket* client = createClientSocket(serverSocketFD);
		acceptedClients[currentClients++] = *client;

		pthread_t threadId;
		pthread_create(&threadId, NULL, handleClientMessages, (void*)client);
		pthread_detach(threadId);
	}
}

int main(int argc, char const* argv[]) {
	signal(SIGINT, handleTerminationSignal);
	signal(SIGTERM, handleTerminationSignal);
	int serverSocketFD = createSocket();
	const struct sockaddr_in* serverAddress = createAddress(IP, PORT);

	int bindResult = bind(serverSocketFD, (struct sockaddr*)serverAddress, sizeof(*serverAddress));
	if (bindResult != 0) {
		printf("Socket binding failed\n");
		exit(1);
	}

	int listenResult = listen(serverSocketFD, MAX_CLIENTS);
	if (listenResult != 0) {
		printf("Server failed to listen\n");
		exit(1);
	}

	printf("Server is listening on %d\n", PORT);

	acceptClientConnections(serverSocketFD);

	shutdown(serverSocketFD, SHUT_RDWR);
	return 0;
}
