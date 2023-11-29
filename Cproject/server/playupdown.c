#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

#include "playupdown.h"

void play_updown(int* clnts, int count)
{
    srand(time(NULL)); 
    int numberToGuess = rand() % 100;
    printf("rand num: %d\n", numberToGuess);

    int attempts = 0;
    char message[500];
    int guessedNumber;
    char response[500];

    for (int i = 0; i < count; i++) {
        send(clnts[i], "Up & Down game START!\n", strlen("Up & Down game START!\n"), 0);
    }
    
    while(1) {
        send(clnts[0], "Enter your guess (0-99): \n", strlen("Enter your guess (0-99): "), 0);
        
        if (recv(clnts[0], message, 500, 0) <= 0) {
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
            snprintf(response, sizeof(response), "Congratulations! You guessed the number in %d attempts.", attempts);
            send(clnts[0], response, strlen(response), 0);
            break;
        } else if (numberToGuess < guessedNumber) {
            send(clnts[0], "Down\n", strlen("Down\n"), 0);
        } else if (numberToGuess > guessedNumber) {
            send(clnts[0], "Up\n", strlen("Up\n"), 0);
        }
    }
}