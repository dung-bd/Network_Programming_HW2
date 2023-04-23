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
#define MAX_DISK_CNT 10

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
        if (tmp < 0 || tmp > 9) {       // Number only
            return -1;
        }
    }
    int port = atoi(cmd_port);
    if(port < 1 || port > 65535) {      // Port range
        return -1;
    }

    return port;
}


int VerifyDeviceName (char * device_name)
{
    for (unsigned int iChar = 0; iChar < strlen(device_name); ++iChar) {
        if (device_name[iChar] == '-' || device_name[iChar] == '_')      // Skip if '-' or '_'
            continue;
        if ( !isalpha(device_name[iChar]) && !isdigit(device_name[iChar]))
            return -1;
    }
    
    return 0;
}


int VerifyDriveSize (char * disk_size)
{
    char check[5] = "GTgt";   // Gg - GB ; Tt - TB
    char * ptr = strpbrk(disk_size, check);
    char buff[8];

    // Capacity Check
    strncpy(buff, disk_size, ptr - disk_size);
    for (unsigned int iChar = 0; iChar < strlen(buff); ++iChar) {
        if ( !isdigit(buff[iChar]) )        // Number Only
            return -1;
    }

    // Storage Measurement Check
    memset(buff, 0, sizeof(buff));
    strncpy(buff, ptr, 2);
    for (unsigned int iChar = 0; iChar < 2; ++iChar) {
        if ( toupper(buff[iChar]) == 'B' || toupper(buff[iChar]) == 'T' || toupper(buff[iChar]) == 'G') {
            continue;
        } else {
            return -1;
        } 
    }

    return 0;
}

int main(int argc, char * argv[])
{
    char buff[MAX_SIZE];

    // Verifying Command
    if (argc != 3) {
        puts("[**ERROR]: Invalid Command.");
        puts("[USAGE]: ./info_client server_IPv4 port");
        exit(1);
    }

    // Verifying Address
    if (VerifyAddress(argv[1])) {
        puts("[**ERROR]: Invalid IPv4 Address");
        exit(1);
    }

    
    int port = VerifyPort(argv[2]);
    if (port == -1) {
        puts("[**ERROR]: Invalid Port.");
        exit(1);
    }

    
    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    
    addr.sin_port = htons(port);

    
    int c_state = connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (c_state == -1) {
        perror("[**ERROR] - connect()");
        exit(1);
    }

    puts("------------------------------------------------");
    puts("Start typing your device's spec");
    puts("Program can only send maximum of 10 disks info");
    puts("------------------------------------------------");

    
    char dvc_name[60], disk_size[12];
    short disk_cnt;
    unsigned short ptr_pos = 0;

    
    while (1) {
        printf("Device's Name: ");
        fgets(dvc_name, sizeof(dvc_name), stdin);
        dvc_name[strcspn(dvc_name, "\n")] = 0;  
        if (VerifyDeviceName(dvc_name)) {
           printf("[**ERROR]: Invalid Device Name. You cannot use blank spaces and special characters.\n");
           continue;
        }
        strcpy(buff, dvc_name);     
        strcat(buff, " ");
        ptr_pos = strlen(buff);
        break;
    }

    while (1) {
        printf("Number of Drives: ");
        scanf("%hd", &disk_cnt);
        getchar();      

        if (disk_cnt > MAX_DISK_CNT || disk_cnt < 0) {
            printf("[**ERROR]: Invalid Number of Drives.\n");
            continue;
        }
        sprintf(buff + ptr_pos, "%hd", disk_cnt);
        strcat(buff, " ");

        int input_cnt = 0;
        while (input_cnt < disk_cnt) {
            char disk_label = 'a';
            ptr_pos = strlen(buff);     

            printf("Drive #%d\n", input_cnt + 1);
            puts("---");
            printf("- Disk Label: ");
            scanf("%c", &disk_label);
            getchar();      

            if ( !isalpha(disk_label) ) {
                printf("[**ERROR]: Invalid Disk Label.\n");
                exit(1);
            }
            sprintf(buff + ptr_pos, "%c", toupper(disk_label));
            ptr_pos = strlen(buff);     

            while (1) {
                printf("- Disk Size: ");
                fgets(disk_size, sizeof(disk_size), stdin);
                disk_size[strcspn(disk_size, "\n")] = 0;   
                if (VerifyDriveSize(disk_size)) {
                    printf("[*ERROR]: Invalid Disk Size Input.\n");
                    exit(1);
                }
                int tmp = strlen(disk_size) - 2;
                for (unsigned int iChar = tmp; iChar < strlen(disk_size); iChar++) {
                    disk_size[iChar] = toupper(disk_size[iChar]);
                }

                strcat(buff, disk_size);
                strcat(buff, " ");
                break;
            }
            puts("----------");
            input_cnt++;
        }
        break;
    }
    puts("Package to Server:");
    puts(buff);
    send(sock_fd, buff, strlen(buff), 0);
    
   
    close(sock_fd);
}