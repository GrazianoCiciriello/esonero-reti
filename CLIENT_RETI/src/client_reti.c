#include "protocollo1.h"

int main() {
#if defined WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("Error at WSAStartup()\n");
        fflush(stdout);
        return -1;
    }
#endif

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        CLEARWINSOCK;
        return -1;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(DEFAULT_IP);
    server_addr.sin_port = htons(DEFAULT_PORT);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        CLOSESOCKET(client_socket);
        CLEARWINSOCK;
        return -1;
    }

    printf("Connected to the server\n");
    fflush(stdout);
    fflush(stdout);

    //welcome msg server
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
        fflush(stdout);
    } else {
        printf("Failed to receive instructions\n");
        fflush(stdout);
        CLOSESOCKET(client_socket);
        CLEARWINSOCK;
        return -1;
    }

    while (1) {
        printf("\nEnter request (type length) or 'q' to quit (e.g., n 8): ");
        fflush(stdout);
        fgets(buffer, BUFFER_SIZE, stdin);

        if (buffer[0] == 'q') break; // exit whit q

        //send request
        send(client_socket, buffer, strlen(buffer), 0);

        //receive
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                printf("Server disconnected\n");
                fflush(stdout);
            } else {
                perror("Error receiving data");
            }
            break;
        }

        buffer[bytes_received] = '\0';
        printf("Password: %s\n", buffer);
        fflush(stdout);
    }

    CLOSESOCKET(client_socket);
    CLEARWINSOCK;
    return 0;
}
