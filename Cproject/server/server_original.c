#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

#include "playupdown.h"
#include "playhangman.h"
#include "playnbaseball.h"
#include "playchosung.h"

// client 구조
struct client_info {
    int socknum; //socket number
    char ip[INET_ADDRSTRLEN]; // ip array (char)
};

// 함수 선언
void *recv_message(void *sock);
void sendmessagetoclients(char *message, int currentValue);

// pthread mutex 초기화 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

// client max
int clnts[100];
// client count
int count = 0;


int main(int argc, char* argv[])
{
    int mysock, yoursock;
    socklen_t your_addr_size;
    int portnum;

    pthread_t sendT, recvT;

    // client_info 구조체 변수 선언
    struct client_info ci;

    // ip 변수 선언
    char ip[INET_ADDRSTRLEN];

    if (argc > 2) {
        printf("arguments over");
        exit(1);
    }

    
    struct sockaddr_in my_addr, your_addr;
    portnum = atoi(argv[1]);
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(portnum);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");     // 컴퓨터 내부 ip

    your_addr_size = sizeof(your_addr);

    // create socket
    mysock = socket(AF_INET, SOCK_STREAM, 0);
    memset(my_addr.sin_zero, '\0', sizeof(my_addr.sin_zero));

    // bind()
    if (bind(mysock, (struct sockaddr *)&my_addr, sizeof(my_addr)) != 0) {
        perror("bind() error");
        exit(1);
    }

    // listen()
    if (listen(mysock, 5) != 0) {
        perror("listen() error");
        exit(1);
    }

    while(1) {
        // accept()
        if ((yoursock = accept(mysock, (struct sockaddr *)&your_addr, &your_addr_size)) < 0) {
            perror("accpet() error");
            exit(1);
        }

        pthread_mutex_lock(&mutex); // mutex lock
        inet_ntop(AF_INET, (struct sockaddr *)&your_addr, ip, INET_ADDRSTRLEN);
        printf("클라이언트 접속: IP 주소=%s\n", ip);

        // connect: server와 count명의 clients
        ci.socknum = yoursock;
        strcpy(ci.ip, ip);
        clnts[count] = yoursock;
        count++;
        pthread_create(&recvT, NULL, recv_message, &ci);
        pthread_mutex_unlock(&mutex); // mutex unlock
    }

    return 0;
}

void *recv_message(void *sock)
{
    // client_info 구조체 변수 선언
    struct client_info ci = *((struct client_info *)sock);

    // max message length
    char message[500];

    int len;
    while ((len = recv(ci.socknum, message, 500, 0)) > 0) {
        message[len] = '\0'; // NULL문자 삽입

        char sliced[20];

        const char *needle = "/";
        char *substring = strstr(message, needle);

        if (substring != NULL) {
            strcpy(sliced, substring);
            printf("play game: %s\n", sliced);

            if (strcmp(sliced, "/updown\n") == 0) {
                play_updown(clnts, count);
            } else if (strcmp(sliced, "/hangman\n") == 0) {
                play_hangman(clnts, count);
            } else if (strcmp(sliced, "/numberbaseball\n") == 0) {
                play_numberbaseball(clnts, count);
            } else if (strcmp(sliced, "/chosung\n") == 0) {
                play_chosung(clnts, count);
            } else {
                sendmessagetoclients("잘못된 입력\n", ci.socknum);
            }
        } else {
            // printf("substring not found\n");
            sendmessagetoclients(message, ci.socknum);
            fputs(message, stdout);
        }

        memset(message, '\0', sizeof(message));
    }

    pthread_mutex_lock(&mutex);
    printf("클라이언트 탈출: IP 주소=%s\n", ci.ip);

    for (int i = 0; i < count; i++) {
        if (clnts[i] == ci.socknum) {
            int j = i;

            while (j < count-1) {
                clnts[j] = clnts[j+1];
                j++;
            }
        }
    }

    count--;
    pthread_mutex_unlock(&mutex);
}

void sendmessagetoclients(char *message, int currentValue)
{
    pthread_mutex_lock(&mutex);

    for (int i = 0; i < count; i++) {
        if (clnts[i] != currentValue) {
            if(send(clnts[i], message, strlen(message), 0) < 0) {
                perror("send fail");
                continue;
            }
        }
    }

    pthread_mutex_unlock(&mutex);
}