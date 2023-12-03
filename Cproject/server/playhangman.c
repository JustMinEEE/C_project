#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#include "playhangman.h"

#define MAX_WORD_LENGTH 20
#define MAX_TRIES 6

// 사용자가 입력한 단어를 저장할 배열
char userWord[MAX_WORD_LENGTH];

// 화면 클리어
void clearScreen() { 
    system("clear");
}

// 단어 선택 및 초기화 함수
void initializeGame(char* word, char* guessedWord) {
    // 단어 복사
    // strcpy(word, userWord);

    // guessedWord 동적 할당
    
    char *token = strtok(userWord, " ");

    token = strtok(NULL, " ");
    strcpy(word, token);

    // guessedWord 초기화
    int length = strlen(word);
    for (int i = 0; i < length-1; i++) {
        (guessedWord)[i] = '_';
    }
    (guessedWord)[length] = '\0';
}

void play_hangman(int* clnts, int count)
{
    
    // clearScreen(); // 게임 시작 전에 화면 클리어

    // 사용자로부터 단어 입력 받기
    send(clnts[0], "\n\n\n\n\n\nServer: Enter a word for the Hangman game: ", strlen("\n\n\n\n\n\nServer: Enter a word for the Hangman game: "), 0);
    send(clnts[1], "\n\n\n\n\n\nServer: Your friend is entering...\n\n\n\n\n", strlen("\n\n\n\n\n\nServer: Your friend is entering...\n\n\n\n\n"), 0);
    
    int len;
    if ((len = recv(clnts[0], userWord, 20, 0)) <= 0) {
            perror("recv");
            return;
    }

    // fgets로 입력받은 개행 문자 처리
    userWord[len] = '\0';

    char word[MAX_WORD_LENGTH];
    char guessedWord[MAX_WORD_LENGTH]; // 초기화

    printf("userWord: %s\n", userWord);
    initializeGame(word, guessedWord);

    int tries = 0;
    int correctGuess = 0;
    printf("word: %s\n", word);
    printf("word len: %ld\n", strlen(word));
    printf("guessedWord: %s\n", guessedWord);
    
    char response[500];
    while (tries < MAX_TRIES && correctGuess < strlen(word)-1) {
        printf("in\n");
        snprintf(response, sizeof(response), "Server: Current word: %s", guessedWord);
        send(clnts[1], response, sizeof(response), 0);
        send(clnts[0], response, sizeof(response), 0);

        snprintf(response, sizeof(response), "Server: Tries left: %d", MAX_TRIES - tries);
        send(clnts[1], response, sizeof(response), 0);
        send(clnts[0], response, sizeof(response), 0);

        char guess[500];
        send(clnts[1], "Server: Enter a letter: ", strlen("Server: Enter a letter: "), 0);
        
        int len;
        if ((len = recv(clnts[1], guess, sizeof(guess), 0)) <= 0) {
            perror("recv");
            break;
        }
        guess[len] = '\0';

        snprintf(response, sizeof(response), "Server: your friend's answer is %s\n", guess);
        send(clnts[0], response, sizeof(response), 0);
        printf("guess: %s\n", guess);
        
        char *token = strtok(guess, " ");

        token = strtok(NULL, " ");
        strcpy(guess, token);

        char temp = guess[0];
        printf("temp: %c\n", temp);


        int found = 0;
        // 정답에 알파벳이 포함되어 있는지 확인
        for (int i = 0; i < strlen(word); i++) {
            if (word[i] == temp) {
                // 알파벳이 정답에 포함되어 있다면 guessedWord 업데이트
                guessedWord[i] = temp;
                found = 1;
                correctGuess++;
            }
        }

        if (!found) {
            // 알파벳이 정답에 없으면 오류 시도 횟수 증가
            tries++;
        }

        printf("correctGuess: %d, tries: %d\n", correctGuess, tries);

        memset(guess, '\0', sizeof(guess));
    }

    printf("out\n");

    if (correctGuess == strlen(word)-1) {
        snprintf(response, sizeof(response), "Server: Congratulations! You guessed the word: %s\n", word);
        send(clnts[1], response, sizeof(response), 0);
        send(clnts[0], "Server: your friend finds the word!!\n", sizeof("Server: your friend finds the word!!\n"), 0);
    } else {
        snprintf(response, sizeof(response), "Server: you ran out of tries. The answer was: %s\n", word);
        send(clnts[1], response, sizeof(response), 0);
        send(clnts[0], "Server: your friend can't find the word!!\n", sizeof("Server: your friend can't find the word!!\n"), 0);  
    }
}