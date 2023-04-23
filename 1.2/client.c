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
#define MAX_SIZE 2048

int main()
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SRV_ADDR);
    addr.sin_port = htons(PORT);

    int c_state = connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (c_state == -1) {
        perror("[**ERROR] - connect()");
        exit(1);
    }

    char buff[MAX_SIZE];
    FILE * f_handler;
    f_handler = fopen("data.txt", "r");
    while ( !feof(f_handler) ) {
        fscanf(f_handler, "%s", buff);      
    }
    puts(buff);
    send(sock_fd, buff, strlen(buff), 0);

    close(sock_fd);
}
