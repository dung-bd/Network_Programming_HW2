#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define RECEIVER_ADDR "127.0.0.1"
#define PORT 9999
#define MAX_SIZE 2048

int VerifyAddress (const char * cmd_address) 
{
    char buff[32], sub_buff[8];
    char * octet_str;
    
    strcpy(buff, cmd_address);
    octet_str = strtok(buff, ".");       

    while (octet_str != NULL) {
        strcpy(sub_buff, octet_str);

        unsigned int oclen = strlen(octet_str);       
        if(oclen > 3) {
            return 1;
        }
        for (unsigned int iDigit = 0; iDigit < oclen; ++iDigit) {
            int tmp = sub_buff[iDigit] - '0';     
            if (tmp < 0 || tmp > 9) {
                return 1;
            }
        }

        octet_str = strtok(NULL, ".");
    }
    
    return 0;
}

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

void ReturnError (char * msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char * argv[])
{
    if (argc != 4) {
        puts("[**ERROR]: Invalid Command");
        puts("[USAGE]: ./upd_sender2 receiver_ipv4 port send_file");
        exit(1);
    }
    
    if (VerifyAddress(argv[1])) {
        puts("[**ERROR]: Invalid IPv4 Address");
        exit(1);
    }

    int port = VerifyPort(argv[2]);
    if (port == -1) {
        puts("[**ERROR]: Invalid Port.");
        exit(1);
    }

    int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(port);

    char buff[MAX_SIZE];
    puts("Sending....");
    while (1) {
        FILE * f_handler;

        f_handler = fopen(argv[3], "r");
        if (f_handler == NULL) {
            ReturnError("[**ERROR] - fopen()");
        }
        fread(buff, sizeof(buff), 1, f_handler);
        fclose(f_handler);

        sendto(sock_fd, buff, strlen(buff), 0, (struct sockaddr *)&addr, sizeof(addr));
        memset(buff, 0, sizeof(buff));
        usleep(3000000);   
    }
}