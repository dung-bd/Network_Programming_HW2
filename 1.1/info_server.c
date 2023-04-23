#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SRV_ADDR "127.0.0.1"
#define PORT 9999
#define MAX_SIZE 256


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
        puts("[**ERROR]: Invalid Command.");
        puts("[USAGE]: ./info_server port");
        exit(1);
    }

    
    int port = VerifyPort(argv[1]);
    if (port == -1) {
        puts("[**ERROR]: Invalid Port.");
        exit(1);
    }

   
    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd == -1) {
        perror("[**ERROR] - socket()");
        exit(1);
    }

   
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    addr.sin_port = htons(port);

 
    if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("[**ERROR] - bind()");
        exit(1);
    }

    
    if (listen(sock_fd, 5)) {
        perror("[**ERROR] - listen()");
        exit(1);
    }
    printf("Listening on port %d...\n", port);

    
    int acpt_client = accept(sock_fd, NULL, NULL);
    if (acpt_client == -1) {
        perror("[**ERROR] - accept()");
        exit(1);
    }    
    puts("New Client Connected.");

    
    char buff[MAX_SIZE];
    puts("Received from Client:");
    puts("----------------------");

    while(1) {
        char sub_buff[20];
        memset(sub_buff, 0, sizeof(sub_buff));     

        int rcv_byte = recv(acpt_client, buff, MAX_SIZE, 0);
        if (rcv_byte <= 0) {
            printf("Connection closed.\n");
            break;
        }
        if (rcv_byte < MAX_SIZE) {
            buff[rcv_byte] = 0;    
        }
            
        
        char * tmp_string  = strtok(buff, " ");
        printf("Device Name: %s\n", tmp_string);

        tmp_string = strtok(NULL, " ");
        if (atoi(tmp_string) == 1) {
            printf("The device has %s disk.\n", tmp_string);
        } else {
            printf("The device has %s disks.\n", tmp_string);
        }

        while ((tmp_string = strtok(NULL, " ")) != NULL) {
            memset(sub_buff, 0, sizeof(sub_buff));      
            strncpy(sub_buff, tmp_string, strlen(tmp_string));
            printf("\t+) %c - %s\n", sub_buff[0], sub_buff + 1);
        
        }
        puts("----------------------");
    }

    close(acpt_client);
    close(sock_fd);
}