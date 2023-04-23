#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 9999
#define MAX_SIZE 65535
#define SAVE_FILE_PATH "receiver_datagram.txt" 

int VerifyPort (const char * cmd_port)
{
    char buff[10];
    strcpy(buff, cmd_port);
    for(unsigned int iBuff = 0; iBuff < strlen(buff); ++iBuff) {
        int tmp = buff[iBuff] - '0';
        if (tmp < 0 || tmp > 9) {       
            return -1;
        }
    }
    int port = atoi(cmd_port);
    if(port < 1 || port > 65535) {     
        return -1;
    }

    return port;
}

int main(int argc, char * argv[])
{
    if (argc != 2) {
        puts("[**ERROR]: Invalid Command");
        puts("[USAGE]: ./udp_receiver port");
        exit(1);
    }

    int port = VerifyPort(argv[1]);
    if (port == -1) {
        puts("[**ERROR]: Invalid Port.");
        exit(1);
    }

    int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("[**ERROR] - bind()");
        exit(1);
    }
    printf("Listening on port %d\n", PORT);
    
    char buff[MAX_SIZE];
    while (1) {
        int recv_byte = recvfrom(sock_fd, buff, sizeof(buff), 0, NULL, NULL);        
        if (recv_byte == -1) {
            puts("recvfrom() failed.");
            break;
        } else {
            buff[recv_byte] = 0;
            char rcv_filename[30];
            int str_ptr = strcspn(buff, " ");

            strncpy(rcv_filename, buff, str_ptr);

            FILE * f_handler;
            f_handler = fopen(SAVE_FILE_PATH, "w");
            puts("--------------------------------");
            printf("File received from Client: %s\n", rcv_filename);;
            puts("Saved to 'receiver_datagram.txt'.");
            puts("--------------------------------");
            puts(buff + str_ptr + 1);
            fprintf(f_handler, "%s", buff + str_ptr + 1);
            fclose(f_handler);

            memset(buff, 0, sizeof(buff));
        }
    }
}