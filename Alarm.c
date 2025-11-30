#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

typedef struct ScheduleNode {
    int day;        // 1~7
    int hour;       // 9~24
    int minute;     // 0~59
    char text[128];
    struct ScheduleNode* next;
} ScheduleNode;

ScheduleNode* head = NULL;

SYSTEMTIME deadlineTime;
int hasDeadline = 0;
int deadlineAlertTriggered = 0;
char* dayName[7] = { "월", "화", "수", "목", "금", "토", "일" };

// ======================= 함수 선언 =======================
void mainMenu();
void loadFile();
void saveFile();
void addScheduleMenu();
void deleteScheduleMenu();
void printAllSchedules();
void showScheduleText();
void alarmMode();
void setDeadline();

// ======================= 링크드 리스트 ======================

// 일정 추가 (중복 시 덮어쓰기)
void addScheduleNode(int day, int hour, int minute, char* text) {
    ScheduleNode* cur = head;

    while (cur != NULL) {
        if (cur->day == day && cur->hour == hour && cur->minute == minute) {
            strcpy(cur->text, text);
            return;
        }
        cur = cur->next;
    }

    ScheduleNode* newNode = (ScheduleNode*)malloc(sizeof(ScheduleNode));
    newNode->day = day;
    newNode->hour = hour;
    newNode->minute = minute;
    strcpy(newNode->text, text);
    newNode->next = head;
    head = newNode;
    printf("Enter를 누르면 메뉴로 돌아갑니다...");
}

// 일정 삭제
void deleteScheduleNode(int day, int hour, int minute) {
    ScheduleNode* cur = head;
    ScheduleNode* prev = NULL;

    while (cur != NULL) {
        if (cur->day == day && cur->hour == hour && cur->minute == minute) {
            if (prev == NULL) head = cur->next;
            else prev->next = cur->next;
            free(cur);
            printf("\n일정이 삭제되었습니다.\n");
            return;
        }
        prev = cur;
        cur = cur->next;
    }

    printf("\n해당 일정이 존재하지 않습니다.\n");
    printf("Enter를 누르면 메뉴로 돌아갑니다...");
}

// 일정 찾기
ScheduleNode* findSchedule(int day, int hour, int minute) {
    ScheduleNode* cur = head;
    while (cur != NULL) {
        if (cur->day == day && cur->hour == hour && cur->minute == minute)
            return cur;
        cur = cur->next;
    }
    return NULL;
    printf("Enter를 누르면 메뉴로 돌아갑니다...");
}

// ======================= 파일 입출력 =======================
void saveFile() {
    FILE* fp = fopen("C_project.txt", "w");
    if (!fp) return;

    ScheduleNode* cur = head;
    while (cur != NULL) {
        fprintf(fp, "%d %d %d %s\n",
            cur->day, cur->hour, cur->minute, cur->text);
        cur = cur->next;
    }

    fclose(fp);
    printf("Enter를 누르면 메뉴로 돌아갑니다...");
}

void loadFile() {
    FILE* fp = fopen("C_project.txt", "r");
    if (!fp) return;

    head = NULL;
    int d, h, m;
    char txt[128];

    while (fscanf(fp, "%d %d %d %[^\n]", &d, &h, &m, txt) == 4) {
        addScheduleNode(d, h, m, txt);
    }

    fclose(fp);
    printf("Enter를 누르면 메뉴로 돌아갑니다...");
}

// ======================= 메뉴 기능 =======================

// 일정 추가
void addScheduleMenu() {
    int day, hour, minute;
    char text[128];

    printf("\n==== 새 일정 입력 ====\n");

    printf("요일 1.월 2.화 3.수 4.목 5.금 6.토 7.일: ");
    scanf("%d", &day);

    printf("시간 (9~24): ");
    scanf("%d", &hour);

    printf("분 (0~59): ");
    scanf("%d", &minute);

    printf("내용 입력: ");
    getchar();
    fgets(text, sizeof(text), stdin);
    text[strcspn(text, "\n")] = 0;

    addScheduleNode(day, hour, minute, text);
    saveFile();

    printf("\n일정이 저장되었습니다!\n");
    printf("Enter를 누르면 메뉴로 돌아갑니다...");
}

// 일정 삭제 메뉴
void deleteScheduleMenu() {
    printf("\n==== 일정 삭제 ====\n");

    ScheduleNode* cur = head;
    int count = 0;

    if (!cur) {
        printf("삭제할 일정이 없습니다.\n");
        printf("Enter를 누르면 메뉴로 돌아갑니다...");
        getchar(); getchar();
        return;
    }

    // 1. 먼저 전체 일정 출력 + 번호 부여
    ScheduleNode* list[500];  // 최대 500개 일정 저장 가능
    printf("\n현재 저장된 일정 목록:\n\n");

    while (cur != NULL) {
        printf("%d. [%s요일 %02d:%02d] %s\n",
            count + 1,
            dayName[cur->day - 1],
            cur->hour,
            cur->minute,
            cur->text
        );

        list[count] = cur; // 노드 주소 저장
        count++;
        cur = cur->next;
    }

    // 2. 삭제 번호 입력 받기
    int sel;
    printf("\n삭제할 일정 번호 입력: ");
    scanf("%d", &sel);

    if (sel < 1 || sel > count) {
        printf("\n잘못된 번호입니다.\n");
        printf("Enter를 누르면 메뉴로 돌아갑니다...");
        getchar(); getchar();
        return;
    }

    // 3. 선택된 노드 삭제
    ScheduleNode* target = list[sel - 1];

    ScheduleNode* prev = NULL;
    cur = head;

    while (cur != NULL) {
        if (cur == target) {
            if (prev == NULL) head = cur->next;
            else prev->next = cur->next;

            free(cur);
            printf("\n[%d번 일정] 삭제 완료!\n", sel);
            saveFile();

            printf("Enter를 누르면 메뉴로 돌아갑니다...");
            getchar(); getchar();
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

// 일정 전체 출력
void printAllSchedules() {
    ScheduleNode* cur = head;

    printf("\n===== 저장된 전체 일정 =====\n");

    if (!cur) {
        printf("저장된 일정이 없습니다.\n");
        return;
    }

    SYSTEMTIME now;
    GetLocalTime(&now);

    int todayWeek = (now.wDayOfWeek == 0 ? 7 : now.wDayOfWeek); // 일요일 보정

    while (cur != NULL) {
        printf("[%s요일 %02d:%02d] %s\n",
            dayName[cur->day - 1],
            cur->hour,
            cur->minute,
            cur->text
        );

        // ======================================
        //   1. 요일 차이 계산 (요일 기반)
        // ======================================
        int dayDiff = (cur->day - todayWeek + 7) % 7;

        // ======================================
        //   2. 같은 요일일 때 (dayDiff == 0)
        // ======================================
        if (dayDiff == 0) {
            // 시간이 이미 지난 경우 → 마감
            if (cur->hour < now.wHour ||
                (cur->hour == now.wHour && cur->minute < now.wMinute)) {

                printf("   → 마감 기한 지남!\n\n");
            }
            else {
                // 남은 시간 계산
                int diffH = cur->hour - now.wHour;
                int diffM = cur->minute - now.wMinute;

                if (diffM < 0) {
                    diffM += 60;
                    diffH--;
                }

                printf("   → 마감까지 남은 시간: %d시간 %d분\n\n",
                    diffH, diffM);
            }

            cur = cur->next;
            continue;
        }

        // ======================================
        //   3. 다른 요일일 때 → 요일 차이 기반 계산
        // ======================================
        // 남은 시간 = (dayDiff * 24시간) + (남은 오늘 시간 + 내일 이후 시간)
        int totalHours = dayDiff * 24;

        int nowTotalMin = now.wHour * 60 + now.wMinute;
        int targetTotalMin = cur->hour * 60 + cur->minute;

        int diffMin = (dayDiff * 1440) + (targetTotalMin - nowTotalMin);

        if (diffMin < 0) {
            printf("   → 마감 기한 지남!\n\n");
        }
        else {
            int days = diffMin / 1440;
            diffMin %= 1440;
            int hours = diffMin / 60;
            int minutes = diffMin % 60;

            printf("   → 마감까지 남은 시간: %d일 %d시간 %d분\n\n",
                days, hours, minutes);
        }

        cur = cur->next;
    }
}





// ======================= 시간표 시각화 =======================
void showScheduleText() {
    printf("\n=== 주간 시간표 ===\n\n");
    printf("      월  화  수  목  금  토  일\n");

    for (int h = 9; h <= 24; h++) {
        printf("%02d시  ", h);
        for (int d = 1; d <= 7; d++) {
            int found = 0;
            ScheduleNode* cur = head;

            while (cur != NULL) {
                if (cur->day == d && cur->hour == h) {
                    found = 1;
                    break;
                }
                cur = cur->next;
            }

            printf(found ? " ●  " : " -  ");
        }
        printf("\n");
    }

    printf("\n● = 일정 있음 / - = 없음\n\n");
}

// ======================= 마감 시간 설정 =======================
void setDeadline() {
    system("cls");
    printf("\n==== 마감 시간 설정 ====\n");

    printf("년 (예: 2025): "); scanf("%hd", &deadlineTime.wYear);
    printf("월 (1~12): "); scanf("%hd", &deadlineTime.wMonth);
    printf("일 (1~31): "); scanf("%hd", &deadlineTime.wDay);
    printf("시 (0~23): "); scanf("%hd", &deadlineTime.wHour);
    printf("분 (0~59): "); scanf("%hd", &deadlineTime.wMinute);

    deadlineTime.wSecond = 0;

    hasDeadline = 1;
    deadlineAlertTriggered = 0;

    printf("\n마감 시간 설정 완료!\n");
}

// ======================= 알람 모드 =======================
void alarmMode() {
    SYSTEMTIME t;

    while (1) {
        GetLocalTime(&t);

        int week = (t.wDayOfWeek == 0 ? 7 : t.wDayOfWeek);

        ScheduleNode* s = findSchedule(week, t.wHour, t.wMinute);

        if (s != NULL) {
            system("cls");
            printf("=====================================\n");
            printf("             일정 알림! \n");
            printf("=====================================\n\n");

            printf("[%d요일 %02d:%02d]\n", week, t.wHour, t.wMinute);
            printf("내용: %s\n\n", s->text);

            printf("Enter를 누르면 메뉴로 돌아갑니다...");
            getchar(); getchar();

            return;
        }

        Sleep(1000);
    }
}

// ======================= 메인 메뉴 =======================
void mainMenu() {
    int choice;

    while (1) {
        system("cls");
        printf("====== 일정 관리 프로그램 ======\n");
        printf("1. 일정 추가\n");
        printf("2. 일정 삭제\n");
        printf("3. 전체 일정 보기\n");
        printf("4. 주간 시간표 보기\n");
        printf("5. 마감 시간 설정\n");
        printf("6. 알람 모드 실행\n");
        printf("7. 종료\n");
        printf("===============================\n");

        printf("선택: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1: addScheduleMenu(); break;
        case 2: deleteScheduleMenu(); break;
        case 3: printAllSchedules(); getchar(); getchar(); break;
        case 4: showScheduleText(); getchar(); getchar(); break;
        case 5: setDeadline(); getchar(); getchar(); break;
        case 6: alarmMode(); break;
        case 7: return;
        default: printf("잘못된 입력입니다."); getchar(); getchar(); break;
        }
    }
}

// ======================= main =======================
int main() {
    loadFile();
    mainMenu();
    saveFile();
    return 0;
}
