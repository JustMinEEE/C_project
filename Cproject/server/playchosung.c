#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>

#include "playchosung.h"

#define WORD_NUM 33   // 단어 개수 미리 정의
#define MAX_PLAYERS 10

int countWords(const char* str) {  // 한 초성에 해당하는 단어 개수 새는 함수
    int wordCount = 0;
    int inWord = 0; // 단어 안에 있는지를 나타내는 플래그

    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == ' ' || str[i] == '\n' || str[i] == '\t') {
            inWord = 0; // 공백이거나 줄바꿈 문자일 경우 단어 안에 있지 않음
        }
        else if (inWord == 0) {
            inWord = 1; // 단어 시작
            wordCount++; // 단어 개수 증가
        }
    }

    return wordCount;
}

void play_chosung(int* clnts, int count) {
    srand(time(NULL));
    char chosung[WORD_NUM][20]; // 초성을 저장할 배열
    char words[WORD_NUM][1500]; // 단어를 저장할 배열
    int scores[MAX_PLAYERS] = { 0 };

    FILE* file = fopen("words.txt", "r");  // 파일 열기

    int i = 0;
    while (fscanf(file, "%s", chosung[i]) == 1) {
        fscanf(file, "%[^\n]s", words[i]);  // 파일에서 초성과 단어 읽어오기
        i++;
    }
    fclose(file);  // 파일 닫기

    char result[WORD_NUM][100][50]; // 새로운 배열

    int wordn[WORD_NUM] = { 0 };  // 초성에 해당하는 단어 개수 저장하는 배열  

    // 주어진 문자열 배열을 단어 단위로 분할하여 새로운 배열에 저장
    for (int i = 0; i < WORD_NUM; i++) {
        int word_count = 0; // 각 행의 단어 개수  
        wordn[i] = countWords(words[i]);     //초성에 해당하는 단어 개수 저장하는 배열에 함수 통해 구한 단어 개수 저장
        char* token = strtok(words[i], " "); // 공백을 기준으로 문자열을 자름

        while (token != NULL && word_count < wordn[i]) {
            strcpy(result[i][word_count], token); // 단어를 새로운 배열에 복사
            token = strtok(NULL, " "); // 다음 단어를 가져옴
            word_count++;
        }
    }
   
    /*for (int i = 0; i < WORD_NUM; i++) {  // 새 배열 출력 예시
        for (int j = 0; j < wordn[i]; j++) {
            printf("%s ", result[i][j]);
        }
        printf("\n");
    }*/

    for (int i = 1; i < 11; i++) {  // 10 라운드 존재
        int r_index = rand() % WORD_NUM;  // 랜덤 인덱스 생성
        int score = count;
        time_t start = time(NULL); // 시작 시간 기록
        char round[100]; // 충분한 크기의 문자열 배열 생성

        sprintf(round, "%d 라운드 - 초성: %s\n", i, chosung[r_index]);
        for (int u = 0; u < count; u++) {
            send(clnts[u], round, strlen(round), 0);
        }
        while (1) {
            char message[50];
        	int brk = 0;
        	char core[100];
        	
            for (int k = 0; k < count; k++) {  // 플레이어들에게
		        int len;
		        if((len = recv(clnts[k], message, 50, 0)) <= 0) {  // 입력받아서
		   	        perror("recv");
		   	        break;
		        }
		        message[len-1] = '\0';  // recv로 받을 때 \n 도 함께 와서 \0으로 바꿔주기

                char *token = strtok(message, " ");

                token = strtok(NULL, " ");
                strcpy(message, token);

		        int tf = 0;
		        printf("%s\n",message);
		        
                for (int v = 0; v < wordn[r_index]; v++) {  // 랜덤 초성에 해당하는 단어 개수만큼 반복
			        if (strcmp(message, result[r_index][v]) == 0) {  // 입력받은 단어와 배열에 있던 단어가 같으면
			   	        scores[k] += score;
				        printf("정답! %d번 플레이어 %d점 획득!\n", k+1, score);
				        printf("%d번 플레이어 : %d점\n\n", k+1, scores[k]);
				        sprintf(core, "%d번 플레이어  정답!\n", k+1);
				        score--;  //늦게 정답 쓸수록 낮은 점수
				        tf = 1;
				        break;
			        }
		        }

		        if (!tf) {
		   	        printf("%d 번 플레이어 오답!\n\n", k+1);
		   	        sprintf(core, "%d번 플레이어  오답!\n", k+1);
		        }

		        for (int b = 0; b < count; b++) {
            	    send(clnts[b], core, strlen(core), 0);  // 본인 제외한 플레이어들에게 오답,정답 메세지 보내 출력
       		    }
		        time_t current = time(NULL);
		   
                if (score == 0) {  // 플레이어 수만큼의 정답 나오면(한 명이 복수 정답 가능)
		            printf("모두 정답! 라운드 종료!\n\n");
		            brk = 1;
		            break;  // 종료
		        } else if (current - start > 60) {  // 시간 초과되면
		   	        printf("시간 초과! 라운드 종료!\n");
		   	        brk = 1;
		   	        break; // 종료
		        }
            }
            if (brk) {
            	break;
            }	
        }
    }
}