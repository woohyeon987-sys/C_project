

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

typedef struct ScheduleNode {
    int day;        
    int hour;       
    int minute;    
    char text[128];
    struct ScheduleNode* next;
} ScheduleNode;

ScheduleNode* head = NULL;


SYSTEMTIME deadlineTime;
int hasDeadline = 0;
int deadlineAlertTriggered_24 = 0;
int deadlineAlertTriggered_12 = 0;
int deadlineAlertTriggered_6 = 0;
int deadlineAlertTriggered_1 = 0;

char* dayName[7] = { "월", "화", "수", "목", "금", "토", "일" };

void mainMenu();
void addScheduleMenu();
void deleteScheduleMenu();
void printAllSchedules();
void showScheduleText();
void setDeadline();
void alarmMode();
void checkDeadlineAlert();
void saveFile();
void loadFile();

void swapNodeData(ScheduleNode* a, ScheduleNode* b);
void sortSchedules();
void addScheduleNode(int day, int hour, int minute, char* text);

// ======================= 정렬을 위한 노드 스왑 =======================
void swapNodeData(ScheduleNode* a, ScheduleNode* b) {
    int t_day = a->day;
    int t_hour = a->hour;
    int t_min = a->minute;
    char t_text[128];
    strcpy(t_text, a->text);

    a->day = b->day;
    a->hour = b->hour;
    a->minute = b->minute;
    strcpy(a->text, b->text);

    b->day = t_day;
    b->hour = t_hour;
    b->minute = t_min;
    strcpy(b->text, t_text);
}

// ======================= 일정 자동 정렬  =======================
void sortSchedules() {
    if (!head) return;

    ScheduleNode* i = head;
    ScheduleNode* j;

    while (i != NULL) {
        j = i->next;
        while (j != NULL) {
            if (i->day > j->day ||
                (i->day == j->day && i->hour > j->hour) ||
                (i->day == j->day && i->hour == j->hour && i->minute > j->minute)) {
                swapNodeData(i, j);
            }
            j = j->next;
        }
        i = i->next;
    }
}

// ======================= 링크드 리스트: 일정 추가 =======================
void addScheduleNode(int day, int hour, int minute, char* text) {
    ScheduleNode* cur = head;

    // 같은 요일·시간·분에 이미 일정이 있으면 덮어쓰기
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

    printf("일정이 추가되었습니다.\n");
}

// ======================= 파일 저장/로드 =======================
void saveFile() {
    FILE* fp = fopen("C_project.txt", "w");
    if (!fp) {
        printf("파일 저장 실패!\n");
        return;
    }

    // 1) 일정 저장
    ScheduleNode* cur = head;
    while (cur != NULL) {
        fprintf(fp, "S %d %d %d %s\n",
            cur->day, cur->hour, cur->minute, cur->text);
        cur = cur->next;
    }

    // 2) 마감 시간 저장
    if (hasDeadline) {
        fprintf(fp, "D %d %d %d %d %d\n",
            deadlineTime.wYear,
            deadlineTime.wMonth,
            deadlineTime.wDay,
            deadlineTime.wHour,
            deadlineTime.wMinute
        );
    }

    fclose(fp);
}


void loadFile() {
    FILE* fp = fopen("C_project.txt", "r");
    if (!fp) return;

    head = NULL;
    hasDeadline = 0;  // 초기화

    char type;
    int d, h, m;
    char txt[128];

    while (fscanf(fp, "%c", &type) == 1) {

        if (type == 'S') {
            fscanf(fp, "%d %d %d %[^\n]", &d, &h, &m, txt);
            addScheduleNode(d, h, m, txt);
        }

        else if (type == 'D') {
            fscanf(fp, "%hd %hd %hd %hd %hd",
                &deadlineTime.wYear,
                &deadlineTime.wMonth,
                &deadlineTime.wDay,
                &deadlineTime.wHour,
                &deadlineTime.wMinute
            );

            deadlineTime.wSecond = 0;
            hasDeadline = 1;
        }
    }

    fclose(fp);
}

// ======================= 일정 추가 메뉴 =======================
void addScheduleMenu() {
    int day, hour, minute;
    char text[128];

    system("cls");
    printf("\n==== 새 일정 입력 ====\n");

    printf("요일 (1=월 2=화 3=수 4=목 5=금 6=토 7=일): ");
    scanf("%d", &day);

    printf("시간 (0~23): ");
    scanf("%d", &hour);

    printf("분 (0~59): ");
    scanf("%d", &minute);

    printf("내용 입력: ");
    getchar(); // 버퍼 비우기
    fgets(text, sizeof(text), stdin);
    text[strcspn(text, "\n")] = 0; // 개행 제거

    addScheduleNode(day, hour, minute, text);
    saveFile();

    printf("\n일정이 저장되었습니다!\n");
    printf("엔터를 누르면 메뉴로 돌아갑니다...");
    getchar();
}

// ======================= 일정 삭제 메뉴 =======================
void deleteScheduleMenu() {
    system("cls");
    printf("\n==== 일정 삭제 ====\n");

    ScheduleNode* cur = head;
    ScheduleNode* list[500];
    int count = 0;

    if (!cur) {
        printf("삭제할 일정이 없습니다.\n");
        printf("엔터를 누르면 메뉴로 돌아갑니다...");
        getchar(); getchar();
        return;
    }

    // 번호 목록 출력
    while (cur != NULL) {
        printf("%d. [%s요일 %02d:%02d] %s\n",
            count + 1,
            dayName[cur->day - 1],
            cur->hour,
            cur->minute,
            cur->text);
        list[count] = cur;
        count++;
        cur = cur->next;
    }

    int sel;
    printf("\n삭제할 일정 번호: ");
    scanf("%d", &sel);

    if (sel < 1 || sel > count) {
        printf("잘못된 번호입니다.\n");
        printf("엔터를 누르면 메뉴로 돌아갑니다...");
        getchar(); getchar();
        return;
    }

    ScheduleNode* target = list[sel - 1];

    cur = head;
    ScheduleNode* prev = NULL;

    while (cur != NULL) {
        if (cur == target) {
            if (prev == NULL) head = cur->next;
            else prev->next = cur->next;

            free(cur);
            printf("\n[%d번 일정] 삭제 완료!\n", sel);
            saveFile();

            printf("엔터를 누르면 메뉴로 돌아갑니다...");
            getchar(); getchar();
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

// ======================= 전체 일정 출력 (표 스타일 + 남은 시간) =======================
void printAllSchedules() {
    system("cls");
    sortSchedules();

    printf("\n┌───────────────────────────────────────────────┐\n");
    printf("│                 전체 일정 목록                │\n");
    printf("└───────────────────────────────────────────────┘\n\n");

    ScheduleNode* cur = head;
    if (!cur) {
        printf("저장된 일정이 없습니다.\n");
        return;
    }

    SYSTEMTIME now;
    GetLocalTime(&now);

    int today = (now.wDayOfWeek == 0 ? 7 : now.wDayOfWeek);

    while (cur != NULL) {
        printf("■ [%s요일 %02d:%02d] %s\n",
            dayName[cur->day - 1], cur->hour, cur->minute, cur->text);

        // 남은 시간 계산
        int dayDiff = (cur->day - today + 7) % 7;

        int nowMin = now.wHour * 60 + now.wMinute;
        int targetMin = cur->hour * 60 + cur->minute;
        int diffMin = dayDiff * 1440 + (targetMin - nowMin);

        if (diffMin < 0) {
            printf("   → 마감 지남\n\n");
        }
        else {
            int days = diffMin / 1440;
            diffMin %= 1440;
            int hours = diffMin / 60;
            int minutes = diffMin % 60;

            printf("   → 남은 시간: %d일 %d시간 %d분\n\n",
                days, hours, minutes);
        }
        cur = cur->next;
    }
    // ▼▼▼ 일정 출력 끝나고 나서 마감 시간 출력 ▼▼▼
    if (hasDeadline) {
        printf("───────────────────────────────────────────────\n");
        printf(" 설정된 마감 시간\n");
        printf("   → %d년 %d월 %d일 %02d:%02d\n",
            deadlineTime.wYear,
            deadlineTime.wMonth,
            deadlineTime.wDay,
            deadlineTime.wHour,
            deadlineTime.wMinute);

        // 남은 시간 계산
        SYSTEMTIME now;
        GetLocalTime(&now);

        FILETIME f1, f2;
        SystemTimeToFileTime(&deadlineTime, &f1);
        SystemTimeToFileTime(&now, &f2);

        ULONGLONG t1 = (((ULONGLONG)f1.dwHighDateTime << 32) | f1.dwLowDateTime);
        ULONGLONG t2 = (((ULONGLONG)f2.dwHighDateTime << 32) | f2.dwLowDateTime);

        long long diffSec = (long long)(t1 - t2) / 10000000;

        if (diffSec <= 0) {
            printf("   → 마감 시간이 지났습니다.\n");
        }
        else {
            int days = diffSec / 86400;
            int hours = (diffSec % 86400) / 3600;
            int minutes = (diffSec % 3600) / 60;

            printf("   → 마감까지 남은 시간: %d일 %d시간 %d분\n", days, hours, minutes);
        }
        printf("───────────────────────────────────────────────\n");
    }

}

// ======================= 주간 시간표 (표 스타일) =======================
void showScheduleText() {
    system("cls");
    printf("\n      ┌────────────── 주간 시간표 ──────────────┐\n");
    printf("      │   월   화   수   목   금   토   일      │\n");
    printf("      └─────────────────────────────────────────┘\n");

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
            printf(found ? "[ ● ] " : "[ - ] ");
        }
        printf("\n");
    }

    printf("\n[●] = 일정 있음 / [ - ] = 없음\n");
}

// ======================= 마감 시간 설정 =======================
void setDeadline() {
    system("cls");
    printf("\n==== 마감 시간 설정 ====\n");

    printf("년 (예: 2025): ");
    scanf("%hd", &deadlineTime.wYear);
    printf("월 (1~12): ");
    scanf("%hd", &deadlineTime.wMonth);
    printf("일 (1~31): ");
    scanf("%hd", &deadlineTime.wDay);
    printf("시 (0~23): ");
    scanf("%hd", &deadlineTime.wHour);
    printf("분 (0~59): ");
    scanf("%hd", &deadlineTime.wMinute);

    deadlineTime.wSecond = 0;

    hasDeadline = 1;
    deadlineAlertTriggered_24 = 0;
    deadlineAlertTriggered_12 = 0;
    deadlineAlertTriggered_6 = 0;
    deadlineAlertTriggered_1 = 0;

    printf("\n마감 시간 설정 완료!\n");
}

// ======================= 마감 알림 (24/12/6/1시간 전) =======================
void checkDeadlineAlert() {
    if (!hasDeadline) return;

    SYSTEMTIME now;
    GetLocalTime(&now);

    FILETIME f1, f2;
    SystemTimeToFileTime(&deadlineTime, &f1);
    SystemTimeToFileTime(&now, &f2);

    ULONGLONG t1 = (((ULONGLONG)f1.dwHighDateTime << 32) | f1.dwLowDateTime);
    ULONGLONG t2 = (((ULONGLONG)f2.dwHighDateTime << 32) | f2.dwLowDateTime);

    long long diffSec = (long long)(t1 - t2) / 10000000; // 100ns → 초

    if (diffSec <= 0) return;

    int hours = (int)(diffSec / 3600);

    if (hours <= 24 && !deadlineAlertTriggered_24) {
        printf("\n[마감 24시간 전!] 제출 준비를 시작하세요.\n");
        deadlineAlertTriggered_24 = 1;
    }
    if (hours <= 12 && !deadlineAlertTriggered_12) {
        printf("\n[마감 12시간 전!] 남은 시간이 줄어들고 있습니다.\n");
        deadlineAlertTriggered_12 = 1;
    }
    if (hours <= 6 && !deadlineAlertTriggered_6) {
        printf("\n[마감 6시간 전!] 서둘러 마무리해야 합니다.\n");
        deadlineAlertTriggered_6 = 1;
    }
    if (hours <= 1 && !deadlineAlertTriggered_1) {
        printf("\n[마감 1시간 전!] 곧 마감입니다!\n");
        deadlineAlertTriggered_1 = 1;
    }
}

// ======================= 알람 모드 =======================
void alarmMode() {
    SYSTEMTIME t;

    while (1) {
        GetLocalTime(&t);
        checkDeadlineAlert();

        int today = (t.wDayOfWeek == 0 ? 7 : t.wDayOfWeek);

        ScheduleNode* s = head;
        while (s != NULL) {

            int targetTotalMin = s->hour * 60 + s->minute;
            int nowTotalMin = t.wHour * 60 + t.wMinute;
            int diff = targetTotalMin - nowTotalMin;

            if (s->day == today && diff <= 10 && diff >= 1) {

                system("cls");

                printf("┌───────────────────────────┐\n");
                printf("│  일정 %d분 전 알림!       │\n", diff);
                printf("└───────────────────────────┘\n\n");

                printf("[%s요일 %02d:%02d] %s\n\n",
                    dayName[s->day - 1], s->hour, s->minute, s->text);

                if (hasDeadline) {
                    FILETIME f1, f2;
                    SystemTimeToFileTime(&deadlineTime, &f1);
                    SystemTimeToFileTime(&t, &f2);

                    ULONGLONG tt1 = (((ULONGLONG)f1.dwHighDateTime << 32) | f1.dwLowDateTime);
                    ULONGLONG tt2 = (((ULONGLONG)f2.dwHighDateTime << 32) | f2.dwLowDateTime);
                    long long diffSec = (long long)(tt1 - tt2) / 10000000;

                    if (diffSec > 0) {
                        int days = diffSec / 86400;
                        int hours = (diffSec % 86400) / 3600;
                        int minutes = (diffSec % 3600) / 60;
                        printf("예정된 일정일 진행하셔야 합니다\n\n");
                        //printf(" 마감까지 남은 시간: %d일 %d시간 %d분\n\n",days, hours, minutes);
                    }
                }

                printf("엔터를 누르면 메뉴로 돌아갑니다...");
                getchar(); getchar();
                return;
            }

            s = s->next;
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
        case 1:
            addScheduleMenu();
            break;
        case 2:
            deleteScheduleMenu();
            break;
        case 3:
            printAllSchedules();
            printf("\n엔터를 누르면 메뉴로 돌아갑니다...");
            getchar(); getchar();
            break;
        case 4:
            showScheduleText();
            printf("\n엔터를 누르면 메뉴로 돌아갑니다...");
            getchar(); getchar();
            break;
        case 5:
            setDeadline();
            printf("\n엔터를 누르면 메뉴로 돌아갑니다...");
            getchar(); getchar();
            break;
        case 6:
            alarmMode();
            break;
        case 7:
            return;
        default:
            printf("잘못된 입력입니다.\n");
            printf("엔터를 누르면 메뉴로 돌아갑니다...");
            getchar(); getchar();
            break;
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
