#include <stdio.h>
#include <string.h>
#include <windows.h>

#define DAYS 5
#define HOURS 16

char* dayName[DAYS] = { "월", "화", "수", "목", "금" };
char schedule[DAYS][HOURS][128];

void loadFile();
int isFileEmpty();
void printSchedule();
void addSchedule();
void saveFile();
void alarmMode();

int main() {
    loadFile();

    if (isFileEmpty()) {
        printf("파일에 저장된 일정이 없습니다.\n");
        printf("먼저 내용을 입력해 주세요.\n");

        addSchedule();
        saveFile();
        return 0;
    }

    int choice;
    printf("\n=== 파일에 저장된 일정이 존재합니다 ===\n");
    printSchedule();

    printf("\n1. 파일에 내용만 저장하고 종료\n");
    printf("2. 정해진 시간에 알림 받기\n");
    printf("3. 일정 수정하기\n");
    printf("선택: ");
    scanf("%d", &choice);

    if (choice == 1) {
        printf("프로그램을 종료합니다.\n");
        return 0;
    }
    else if (choice == 2) {
        alarmMode();
    }
    else if (choice == 3) {
        addSchedule();
        saveFile();
        printf("저장 완료! 프로그램 종료.\n");
    }

    return 0;
}

// ---------------------- 파일 로드 ----------------------
void loadFile() {
    FILE* fp = fopen("C_project.txt", "r");

    if (!fp) {
        fp = fopen("C_project.txt", "w");
        fclose(fp);
        return;
    }

    char buffer[256];

    for (int d = 0; d < DAYS; d++) {
        for (int h = 0; h < HOURS; h++) {
            if (fgets(buffer, sizeof(buffer), fp) != NULL) {
                buffer[strcspn(buffer, "\n")] = 0;
                strcpy(schedule[d][h], buffer);
            }
        }
    }

    fclose(fp);
}

// ---------------------- 파일 비어있나? ----------------------
int isFileEmpty() {
    for (int d = 0; d < DAYS; d++) {
        for (int h = 0; h < HOURS; h++) {
            if (strlen(schedule[d][h]) > 0) return 0;
        }
    }
    return 1;
}

// ---------------------- 일정 출력 ----------------------
void printSchedule() {
    printf("\n===== 저장된 일정 =====\n");

    for (int d = 0; d < DAYS; d++) {
        for (int h = 0; h < HOURS; h++) {
            if (strlen(schedule[d][h]) > 0)
                printf("[%s %02d시] %s\n", dayName[d], h + 9, schedule[d][h]);
        }
    }
}

// ---------------------- 일정 입력 ----------------------
void addSchedule() {
    int day;
    int hour;
    char text[128];

    printf("\n==== 새 일정 입력 ====\n");

    printf("요일 선택 (1=월 2=화 3=수 4=목 5=금): ");
    scanf("%d", &day);

    if (day < 1 || day > 5) {
        printf("잘못된 요일입니다.\n");
        return;
    }

    printf("시간 입력 (09~24): ");
    scanf("%d", &hour);

    if (hour < 9 || hour > 24) {
        printf("잘못된 시간입니다.\n");
        return;
    }

    printf("내용 입력: ");
    getchar();
    fgets(text, sizeof(text), stdin);
    text[strcspn(text, "\n")] = 0;

    strcpy(schedule[day - 1][hour - 9], text);

    printf("일정이 저장 준비 되었습니다.\n");
}

// ---------------------- 저장 ----------------------
void saveFile() {
    FILE* fp = fopen("C_project.txt", "w");

    for (int d = 0; d < DAYS; d++) {
        for (int h = 0; h < HOURS; h++) {
            fprintf(fp, "%s\n", schedule[d][h]);
        }
    }

    fclose(fp);
}

// ---------------------- 알림 모드 ----------------------
void alarmMode() {
    SYSTEMTIME t;

    printf("알림 모드로 전환합니다.\n");

    while (1) {
        GetLocalTime(&t);

        int week = t.wDayOfWeek;

        if (week >= 1 && week <= 5) {
            int dIndex = week - 1;
            int hIndex = t.wHour - 9;

            if (hIndex >= 0 && hIndex < HOURS) {
                // 초 완전 제거 → 시/분만 체크
                if (strlen(schedule[dIndex][hIndex]) > 0 &&
                    t.wMinute == 0) // 정각에만 울림
                {
                    MessageBox(NULL, schedule[dIndex][hIndex],
                        TEXT("시간표 알림"), MB_OK);
                }
            }
        }

        Sleep(5000); // 5초마다 체크
    }
}
