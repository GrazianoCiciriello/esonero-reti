#include "protocollo1.h"

//fun psw
void generate_numeric(char *password, int length) {
    const char *digits = "0123456789";
    for (int i = 0; i < length; i++) {
        password[i] = digits[rand() % 10];
    }
    password[length] = '\0';
}

void generate_alpha(char *password, int length) {
    const char *letters = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < length; i++) {
        password[i] = letters[rand() % 26];
    }
    password[length] = '\0';
}

void generate_mixed(char *password, int length) {
    const char *chars = "abcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < length; i++) {
        password[i] = chars[rand() % 36];
    }
    password[length] = '\0';
}


void generate_secure(char *password, int length) {
    const char *chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    for (int i = 0; i < length; i++) {
        password[i] = chars[rand() % 70];
    }
    password[length] = '\0';
}


//welcome msg
void send_instructions(int client_socket) {
    const char *instructions =
        "Welcome to the Password Generator!\n"
        "To request a password, use the format: <type> <length (min 6 max 32)>\n"
        "------ 'n': Numeric password ------\n"
        "------ 'a': Alphabetic password ------\n"
        "------ 'm': Mixed ------\n"
        "------ 's': Secure ------\n"
        "Example: n9\n";

    int bytes_sent = send(client_socket, instructions, strlen(instructions), 0);
    if (bytes_sent < 0) {
        perror("Failed to send instructions");
    }
}

int main() {
#if defined WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("Error at WSAStartup()\n");
        fflush(stdout);
        return -1;
    }
#endif
    //creation socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        CLEARWINSOCK;
        return -1;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DEFAULT_PORT);
    server_addr.sin_addr.s_addr = inet_addr(DEFAULT_IP);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        CLOSESOCKET(server_socket);
        CLEARWINSOCK;
        return -1;
    }
    // queue max 5 (qlen)
    int qlen=5;
    if (listen(server_socket, qlen) < 0) {
        perror("Listen failed");
        CLOSESOCKET(server_socket);
        CLEARWINSOCK;
        return -1;
    }

    printf("Server listening on %s:%d\n", DEFAULT_IP, DEFAULT_PORT);
    fflush(stdout);
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        printf("New connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        fflush(stdout);
        send_instructions(client_socket);//send welcome msg

        char buffer[BUFFER_SIZE];
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

            if (bytes_received <= 0) {
                printf("Client disconnected\n");
                fflush(stdout);
                break;
            }

            buffer[bytes_received] = '\0';
            char type;
            int length;
            //wrong number
            if (sscanf(buffer, "%c %d", &type, &length) != 2 || length < 6 || length > 32) {
                send(client_socket, "Invalid request: use format <type> <length>\n", 44, 0);
                continue;
            }
            //wrong type
            char password[33];
            switch (type) {
                case 'n': generate_numeric(password, length); break;
                case 'a': generate_alpha(password, length); break;
                case 'm': generate_mixed(password, length); break;
                case 's': generate_secure(password, length); break;
                default:
                    send(client_socket, "Invalid type: use 'n', 'a', 'm', or 's'\n", 40, 0);
                    continue;
            }

            send(client_socket, password, strlen(password), 0);
        }

        CLOSESOCKET(client_socket);
    }

    CLOSESOCKET(server_socket);
    CLEARWINSOCK;
    return 0;
}
