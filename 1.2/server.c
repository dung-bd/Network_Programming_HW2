#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 9999
#define MAX_SIZE 2048
#define SUB_STRING "0123456789"

int main()
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd == -1) {
        perror("[**ERROR] - socket()");
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    addr.sin_port = htons(PORT);

    if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("[**ERROR] - bind()");
        exit(1);
    }

    if (listen(sock_fd, 5)) {
        perror("[**ERROR] - listen()");
        exit(1);
    }
    printf("Listening on port %d...\n", PORT);

    int acpt_client = accept(sock_fd, NULL, NULL);
    puts("New client connected."); 
    puts("Data received from client:");

    char buff[MAX_SIZE];
    int occurrence = 0;
    while(1) {
        int rcv_byte = recv(acpt_client, buff, MAX_SIZE, 0);
        if (rcv_byte <= 0) {
            printf("Connection closed.\n");
            break;
        }
        if (rcv_byte < MAX_SIZE) 
            buff[rcv_byte] = 0;

        puts("--------------------------------------------------");
       
        for (unsigned int iChar = 0; iChar < strlen(buff); ++iChar) {
            putchar(buff[iChar]);
            if ((iChar + 1) % 50 == 0) {   
                putchar('\n');
            }
        }
        putchar('\n');
        puts("--------------------------------------------------");
        char * tmp = strstr(buff, SUB_STRING);
        occurrence++;
        
        while ((tmp = strstr(tmp + 1, SUB_STRING))) {
            occurrence++;
        }
        printf("- Substring: %s\n", SUB_STRING);
        printf("- Substring occurrences: %d\n", occurrence);
    }
    
    
    close(acpt_client);
    close(sock_fd);
}

