#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <ctype.h>
#include <time.h>

#include "playnbaseball.h"

// ���� �Ұ� �Լ�
void game_introduce(int* clnts, int count)
{
    char response[1000];
    snprintf(response, sizeof(response), "+------------------------------------------------+\n|                                                |\n|      숫자야구 게임에 오신 것을 환영합니다!       |\n|  숫자야구 게임의 규칙에 대해 설명해드리겠습니다. |\n|  사용자가 먼저 세자리의 초기숫자를 입력합니다. |\n|    그리고 사용자는 3가지 숫자를 추측합니다.    |\n|      자리수와 숫자가 맞으면 '스트라이크(S)',   |\n|   자리수는 틀리고 숫자는 맞으면 '볼(B)'입니다. |\n|     당신에게 주어진 기회는 총 9번입니다.      |\n|                                                |\n+------------------------------------------------+\n");
    
    for(int i = 0; i < count; i++) {
        send(clnts[i], response, strlen(response), 0);
    }
    
    // printf("+------------------------------------------------+\n");
    // printf("|                                                |\n");
    // printf("|      숫자야구 게임에 오신 것을 환영합니다!       |\n");
    // printf("|  숫자야구 게임의 규칙에 대해 설명해드리겠습니다. |\n");
    // printf("|  사용자가 먼저 세자리의 초기숫자를 입력합니다. |\n");
    // printf("|    그리고 사용자는 3가지 숫자를 추측합니다.    |\n");
    // printf("|      자리수와 숫자가 맞으면 '스트라이크(S)',   |\n");
    // printf("|   자리수는 틀리고 숫자는 맞으면 '볼(B)'입니다. |\n");
    // printf("|     당신에게 주어진 기회는 총 9번입니다.      |\n");
    // printf("|                                                |\n");
    // printf("+------------------------------------------------+\n");
}

// �����(1)�� ���� �ʱ� ���ڸ� �Է��ϴ� �Լ�
int get_user_input(char* player_name, int* clnts, int count)
{
    int user_number;
    int user_number_1, user_number_2, user_number_3;

    int i;
    if (strcmp("첫 번째", player_name) == 0) {
        i = 0;
    } else if (strcmp("두 번째", player_name) == 0) {
        i = 1;
    }

    while (1)
    {
        char response[500];
        char message[500];
        snprintf(response, sizeof(response), "%s 플레이어는 초기 숫자(세 자릿수)를 입력하세요.\n", player_name);
        send(clnts[i], response, strlen(response), 0);
        // printf("%s 플레이어는 초기 숫자(세 자릿수)를 입력하세요.\n", player_name);

        if (recv(clnts[i], message, 500, 0) <= 0) {
            perror("recv");
            break;
        }

        char *token = strtok(message, " ");

        token = strtok(NULL, " ");
        strcpy(message, token);

        int number = atoi(message);
        printf("init number: %d\n", number);

        user_number_1 = number % 10;
        number /= 10;
        user_number_2 = number % 10;
        number /= 10;
        user_number_3 = number % 10;
        //scanf("%1d%1d%1d", &user_number_1, &user_number_2, &user_number_3);

        memset(message, '\0', sizeof(message));

        if (user_number_1 != user_number_2 && user_number_1 != user_number_3 && user_number_2 != user_number_3)
        {
            break;
        }
        else
        {
            send(clnts[i], "중복된 숫자를 입력하셨습니다. 다시 입력해주세요.\n", strlen("중복된 숫자를 입력하셨습니다. 다시 입력해주세요.\n"), 0);
        }
    }

    user_number = user_number_1 * 100 + user_number_2 * 10 + user_number_3;
    return user_number;
}

// ����ڰ� ���ߵ� 3�ڸ� ���ڸ� �Է¹޴� �Լ�
int get_user_guess(char* player_name, int* clnts, int count)
{
    int user_guess_1, user_guess_2, user_guess_3;
    int user_guess;

    int i;
    if (strcmp("첫 번째", player_name) == 0) {
        i = 0;
    } else if (strcmp("두 번째", player_name) == 0) {
        i = 1;
    }

    char response[500];
    snprintf(response, sizeof(response), "%s 플레이어는 예상 숫자(세 자릿수)를 입력하세요.\n", player_name);
    send(clnts[i], response, strlen(response), 0);
    // printf("%s 플레이어는 숫자를 유추하세요!\n", player_name);
    // printf("3개의 숫자를 입력해 주세요: ");

    // scanf("%1d%1d%1d", &user_guess_1, &user_guess_2, &user_guess_3);
    char message[500];
    if (recv(clnts[i], message, 500, 0) <= 0) {
        perror("recv");
        return -1;
    }

    char *token = strtok(message, " ");

    token = strtok(NULL, " ");
    strcpy(message, token);

    int number = atoi(message);
    printf("%s init number: %d\n", player_name, number);

    user_guess_1 = number % 10;
    number /= 10;
    user_guess_2 = number % 10;
    number /= 10;
    user_guess_3 = number % 10;

    user_guess = user_guess_1 * 100 + user_guess_2 * 10 + user_guess_3;
    return user_guess;
}

// ���� �� �Լ�
int compare_numbers(int target_number, int guess_number, int* clnts, int turn)
{
    int target_1 = target_number / 100;
    int target_2 = target_number / 10 % 10;
    int target_3 = target_number % 10;

    int guess_1 = guess_number / 100;
    int guess_2 = guess_number / 10 % 10;
    int guess_3 = guess_number % 10;

    int strike = 0, ball = 0;

    char response[500];

    if (target_1 == guess_1)
    {
        strike++;
    }
    if (target_2 == guess_2)
    {
        strike++;
    }
    if (target_3 == guess_3)
    {
        strike++;
    }

    if (target_1 == guess_2 || target_1 == guess_3)
    {
        ball++;
    }
    if (target_2 == guess_1 || target_2 == guess_3)
    {
        ball++;
    }
    if (target_3 == guess_1 || target_3 == guess_2)
    {
        ball++;
    }

    if (strike > 0 && ball > 0)
    {
        snprintf(response, sizeof(response), ">> 스트라이크 %d개, 볼 %d개 입니다.\n", strike, ball);
        send(clnts[turn], response, strlen(response), 0);
        printf(">> 스트라이크 %d개, 볼 %d개 입니다.\n", strike, ball);
    }

    if (strike > 0 && ball == 0 && strike != 3)
    {   
        snprintf(response, sizeof(response), ">> 스트라이크 %d개 입니다.\n", strike);
        send(clnts[turn], response, strlen(response), 0);
        printf(">> 스트라이크 %d개 입니다.\n", strike);
    }

    if (strike == 0 && ball > 0)
    {
        snprintf(response, sizeof(response), ">> 볼 %d개 입니다.\n", ball);
        send(clnts[turn], response, strlen(response), 0);
        printf(">> 볼 %d개 입니다.\n", ball);
    }

    if (strike == 0 && ball == 0)
    {   
        send(clnts[turn], ">> 아웃입니다! 다시 생각하세요\n", strlen(">> 아웃입니다! 다시 생각하세요!\n"), 0);
        printf(">> 아웃입니다! 다시 생각하세요!\n");
    }

    if (strike == 3 && ball == 0)
    {
        snprintf(response, sizeof(response), ">> %d개의 스트라이크입니다. ", strike);
        send(clnts[turn], response, strlen(response), 0);
        send(clnts[turn], "현재 플레이어가 성공적으로 상대의 숫자를 맞췄습니다!\n", strlen("현재 플레이어가 성공적으로 상대의 숫자를 맞췄습니다!\n"), 0);
        printf(">> %d개의 스트라이크입니다. ", strike);
        printf("현재 플레이어가 성공적으로 상대의 숫자를 맞췄습니다!\n");
        return 1; // �¸��� ��Ÿ���� �� ��ȯ
    }
    send(clnts[turn], "==================================================\n", strlen("==================================================\n"), 0);
    return 0; // �¸��� �ƴ� ��� ��� ����
}

// ���� �Լ�
void play_numberbaseball(int* clnts, int count)
{
    int cnt_first_user = 0;  // ù ��° �÷��̾��� �õ� Ƚ�� ���� ����
    int cnt_second_user = 0; // �� ��° �÷��̾��� �õ� Ƚ�� ���� ����
    game_introduce(clnts, count);        // ���� �Ұ� �Լ� ����

    char message[500];

    // ù ��° �÷��̾��� �ʱ� ���� ����
    send(clnts[0], "첫 번째\n", strlen("첫 번째\n"), 0);
    int first_user_number = get_user_input("첫 번째", clnts, count);
    

    // �� ��° �÷��̾��� �ʱ� ���� ����
    send(clnts[1], "두 번째\n", strlen("두 번째\n"), 0);
    int second_user_number = get_user_input("두 번째", clnts, count);
    

    while (1)
    {   
        char response[500];
        cnt_first_user++;
        snprintf(response, sizeof(response), "첫 번째 플레이어의  %d번째 유추시도입니다.\n", cnt_first_user);
        send(clnts[0], response, strlen(response), 0);
        // printf(response);

        int first_user_guess = get_user_guess("첫 번째", clnts, count);
       
        int result = compare_numbers(second_user_number, first_user_guess, clnts, 0);
        if (result == 1)
        {   
            for (int i = 0; i < count; i++) {
                send(clnts[i], "==================================================\n", strlen("==================================================\n"), 0);
                send(clnts[i], "첫 번째 플레이어가 승리하셨습니다! 축하합니다.\n", strlen("첫 번째 플레이어가 승리하셨습니다! 축하합니다.\n"), 0);
            }
            printf("==================================================\n");
            printf("첫 번째 플레이어가 승리하셨습니다! 축하합니다.\n");
            cnt_first_user = 0;
            break;
        }
        if (cnt_first_user == 9)
        {
            snprintf(response, sizeof(response), "정답은 %d%d%d이었습니다!\n", second_user_number / 100, second_user_number / 10 % 10, second_user_number % 10);
            for (int i = 0; i < count; i++) {
                send(clnts[i], "아쉽게도 실패했습니다...\n", strlen("아쉽게도 실패했습니다...\n"), 0);
                send(clnts[i], response, strlen(response), 0);
            }
            break;
        }

        cnt_second_user++;
        snprintf(response, sizeof(response), "두 번째 플레이어의  %d번째 유추시도입니다.\n", cnt_second_user);
        send(clnts[1], response, strlen(response), 0);
        printf("두 번째 플레이어의 %d번째 유추시도입니다. ", cnt_second_user);
        int second_user_guess = get_user_guess("두 번째", clnts, count);

        result = compare_numbers(first_user_number, second_user_guess, clnts, 1);
        if (result == 1)
        {
            for (int i = 0; i < count; i++) {
                send(clnts[i], "==================================================\n", strlen("==================================================\n"), 0);
                send(clnts[i], "두 번째 플레이어가 승리하셨습니다! 축하합니다.\n", strlen("두 번째 플레이어가 승리하셨습니다! 축하합니다.\n"), 0);
            }
            printf("==================================================\n");
            printf("두 번째 플레이어가 승리하셨습니다. 축하합니다.\n");
            cnt_second_user = 0;
            break;
        }
        if (cnt_second_user == 9)
        {
            snprintf(response, sizeof(response), "정답은 %d%d%d이었습니다!\n", second_user_number / 100, second_user_number / 10 % 10, second_user_number % 10);
            for (int i = 0; i < count; i++) {
                send(clnts[i], "아쉽게도 실패했습니다...\n", strlen("아쉽게도 실패했습니다...\n"), 0);
                send(clnts[i], response, strlen(response), 0);
            }
            printf("아쉽게도 실패했습니다...\n");
            printf("정답은 %d%d%d이었습니다!\n", first_user_number / 100, first_user_number / 10 % 10, first_user_number % 10);
            break;
        }
    }
    return;
}
