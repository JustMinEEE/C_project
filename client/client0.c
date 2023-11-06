#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>


// 사용 함수
void *recv_message(void *sock);

// username 변수
char username[100];

int main(int argc, char *argv[]) 
{
    struct sockaddr_in your_addr;

    int mysock, yoursock;
    int your_addr_size;

    char message[500];
    int portnum;
    
    pthread_t sendT, recvT;

    char resource[600];
    char ip[INET_ADDRSTRLEN];

    if (argc > 3) {
        printf("arguments over");
        exit(1);
    }

    portnum = atoi(argv[2]);
    strcpy(username, argv[1]);
    mysock = socket(AF_INET, SOCK_STREAM, 0);
    memset(your_addr.sin_zero, '\0', sizeof(your_addr.sin_zero));
    your_addr.sin_family = AF_INET;
    your_addr.sin_port = htons(portnum);
    your_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(mysock, (struct sockaddr *)&your_addr, sizeof(your_addr)) < 0) {
        perror("connection error");
        exit(1);
    }

    inet_ntop(AF_INET, (struct sockaddr *)&your_addr, ip, INET_ADDRSTRLEN);

    printf("채팅방 접속: IP 주소=%s\n", ip);
    printf("----------------------------------\n");
    printf("my ID: %s\n", username);
    printf("//////////////////////////////////\n");
    printf("me(%s): \n", username);

    pthread_create(&recvT, NULL, recv_message, &mysock);

    while (fgets(message, 500, stdin) > 0) {
        printf("\nme(%s): \n", username);
        strcpy(resource, username);
        strcat(resource, ": ");
        strcat(resource, message);

        int len = write(mysock, resource, strlen(resource));

        if (len < 0) {
            perror("send fail");
            exit(1);
        }

        memset(message, '\0', sizeof(message));
        memset(resource, '\0', sizeof(resource));
    }

    pthread_join(recvT, NULL);
    close(mysock);

    return 0;
}

void *recv_message(void *sock)
{
    int yoursock = *((int *)sock);

    char message[500];

    int len;
    while((len = recv(yoursock, message, 500, 0)) > 0) {
        message[len] = '\0';

        printf("# 다른 사람의 메시지를 기다리는 중...\n\n");

        fputs(message, stdout);

        printf("\nme(%s): \n", username);
        memset(message, '\0', sizeof(message));
    }
}