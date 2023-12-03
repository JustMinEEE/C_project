#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <ctype.h>

#include "playupdown.h"

void play_updown(int* clnts, int count)
{
    // system("clear");
    
    srand(time(NULL)); 
    int numberToGuess = rand() % 100;
    printf("rand num: %d\n", numberToGuess);

    int attempts = 0;
    char message[500];
    int guessedNumber;
    char response[500];

    for (int i = 0; i < count; i++) {
        send(clnts[i], "\n\n\n\n\n\nServer: Up & Down game START!\n", strlen("\n\n\n\n\n\nServer: Up & Down game START!\n"), 0);
        printf("i: %d\n", i);
    }
    
    int i, j;
    while(1) {
        if (attempts % 2 == 0){
            // printf("attempts: %d\n", attempts);
            i = 0;
            j = 1;
        } else {
            i = 1; 
            j = 0;
        }
        // printf("i: %d, j: %d\n", i, j);

        send(clnts[i], "\n\nServer: Enter your guess (0-99): \n", strlen("\n\nServer: Enter your guess (0-99): "), 0);
        send(clnts[j], "\n\nServer: your friend is entering...\n", strlen("\n\nServer: your friend is entering...\n"), 0);

        if (recv(clnts[i], message, 500, 0) <= 0) {
            perror("recv");
            break;
        }
        
        char *token = strtok(message, " ");

        token = strtok(NULL, " ");
        strcpy(message, token);

        guessedNumber = atoi(message);
        printf("guessed Number: %d\n", guessedNumber);
        attempts++;

        if(numberToGuess == guessedNumber) {
            snprintf(response, sizeof(response), "Server: Congratulations! You guessed the number in %d attempts.\n", attempts);
            send(clnts[i], response, strlen(response), 0);

            snprintf(response, sizeof(response), "Server: your friend's answer was %d\n", guessedNumber);
            send(clnts[j], response, strlen(response), 0);
            send(clnts[j], "Server: U lose!!!\n", strlen("Server: U lose!!!\n"), 0);
            break;
        } else if (numberToGuess < guessedNumber) {
            send(clnts[i], "\n\nServer: Down\n", strlen("\n\nServer: Down\n"), 0);

            snprintf(response, sizeof(response), "Server: your friend's answer was %d\n", guessedNumber);
            send(clnts[j], response, strlen(response), 0);
            send(clnts[j], "\n\nServer: Down\n", strlen("\n\nServer: Down\n"), 0);
        } else if (numberToGuess > guessedNumber) {
            send(clnts[i], "\n\nServer: Up\n", strlen("\n\nServer: Up\n"), 0);

            snprintf(response, sizeof(response), "Server: your friend's answer was %d\n", guessedNumber);
            send(clnts[j], response, strlen(response), 0);
            send(clnts[j], "\n\nServer: Up\n", strlen("\n\nServer: Up\n"), 0);
        }

        memset(message, '\0', sizeof(message));
    }
}