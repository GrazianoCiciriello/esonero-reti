#ifndef PROTOCOLLO1_H
#define PROTOCOLLO1_H

#if defined WIN32
#include <winsock.h>
typedef int socklen_t; // Definizione per Windows
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEFAULT_PORT 57015
#define DEFAULT_IP "127.0.0.1"
#define BUFFER_SIZE 1024

#if defined WIN32
#define CLOSESOCKET closesocket
#define CLEARWINSOCK WSACleanup()
#else
#define CLOSESOCKET close
#define CLEARWINSOCK
#endif

void generate_numeric(char *password, int length);
void generate_alpha(char *password, int length);
void generate_mixed(char *password, int length);
void generate_secure(char *password, int length);

#endif
