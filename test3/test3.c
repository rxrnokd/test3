#define _CRT_SECURE_NO_WARNINGS  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <windows.h>  
#include <time.h>  

typedef struct {
    char id[10];
    char password[20];
} userinf;

typedef struct {
    int car;
    int elec;
    int disposable;
    double total;
    struct tm today;
} userdat;

void first_window(userdat* data, userinf* user);

int main()
{
    userinf user1;
    userdat data;

    first_window(&data, &user1);

    return 0;
}

void today(struct tm* t)
{
    time_t now = time(NULL);
    localtime_s(t, &now);

    t->tm_year += 1900;
    t->tm_mon += 1;
}

void data_store(userdat* data, const char* id)
{
    int cnt;
    char filename[20];
    snprintf(filename, sizeof(filename), "%s.dat", id);

    FILE* fp = fopen(filename, "rb+");
    if (fp == NULL)
    {
        fp = fopen(filename, "wb+");
        if (fp == NULL)
        {
            perror("파일 열기 실패");
            return;
        }
        cnt = 1;
    }
    else
    {
        if (fread(&cnt, sizeof(int), 1, fp) != 1)
        {
            cnt = 1;
        }
        else
        {
            cnt++;
        }
    }

    fseek(fp, 0, SEEK_SET);
    fwrite(&cnt, sizeof(int), 1, fp);

    fseek(fp, 0, SEEK_END);
    if (fwrite(data, sizeof(userdat), 1, fp) != 1) {
        perror("데이터 저장 실패");
    }

    fclose(fp);
}

userdat* Import_Data(const char* id, int* count)
{
    int cnt;
    char filename[20];
    snprintf(filename, sizeof(filename), "%s.dat", id);

    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("파일 열기 실패\n");
        return NULL;
    }
    fread(&cnt, sizeof(int), 1, fp);

    userdat* data = (userdat*)malloc(sizeof(userdat) * cnt);

    if (data == NULL) {
        printf("메모리 할당 실패\n");
        fclose(fp);
        return NULL;
    }
    fread(data, sizeof(userdat), cnt, fp);
    //printf("cnt: %d\n", cnt);
    *count = cnt;
    fclose(fp);
    return data;
}

void carbon_emissions_warning(userdat* data)
{

    if (data->total > 1000)
    {
        printf("경고: 탄소 배출량이 1000kg을 초과했습니다!\n");
    }
    else if (data->total > 500)
    {
        printf("주의: 탄소 배출량이 500kg을 초과했습니다!\n");
    }
}

void Total_carbon_emissions(userdat* data)
{
    double car_emission = 0.2 * data->car;
    double elec_emission = 0.424 * data->elec;
    double disposable_emission = 0.1 * data->disposable;

    data->total = car_emission + elec_emission + disposable_emission;

    printf("자동차: %d km x 0.2 = %.2f kg\n", data->car, car_emission);
    printf("전기: %d kWh x 0.424 = %.2f kg\n", data->elec, elec_emission);
    printf("일회용품: %d 개 x 0.1 = %.2f kg\n", data->disposable, disposable_emission);
    printf("\n");
    printf("총 배출량 %.2f + %.2f + %.2f = %.2f kg CO2\n", car_emission, elec_emission, disposable_emission, data->total);
}

void today_emmission(userdat* data, const char* id)
{
    printf("----오늘의 탄소 발자국 입력----\n");
    printf("오늘 자동차 이동거리(km): ");
    scanf_s("%d", &(data->car));
    printf("오늘 전기 사용량(kWh): ");
    scanf_s("%d", &(data->elec));
    printf("오늘 일회용품 사용 개수: ");
    scanf_s("%d", &(data->disposable));
    printf("\n");
    printf("입력 완료!\n");
    Total_carbon_emissions(data);
    printf("\n");
    carbon_emissions_warning(data);
    today(&(data->today));
    data_store(data, id);
}

void cumulative_statistics(const char* id)
{
    int cnt;
    userdat* data = Import_Data(id, &cnt);
    if (data == NULL || cnt == 0) {
        printf("데이터를 불러올 수 없습니다.\n");
        return;
    }
    double sum = 0;
    int maxi = 0, mini = 0;
    for (int i = 0; i < cnt; i++)
    {
        if (data[i].total > data[maxi].total)
        {
            maxi = i;
        }
        if (data[i].total < data[mini].total)
        {
            mini = i;
        }
        sum += data[i].total;
    }

    printf("지금까지 총 탄소 배출량: %.1f kg CO2\n", sum);
    printf("일평균 탄소 배출량: %.1f kg CO2\n", sum / cnt);
    printf("지금까지의 하루 최대 배출량: %.1f kg CO2 %d년 %d월 %d일\n", data[maxi].total, data[maxi].today.tm_year, data[maxi].today.tm_mon, data[maxi].today.tm_mday);
    printf("지금까지의 하루 최소 배출량: %.1f kg CO2 %d년 %d월 %d일\n", data[mini].total, data[mini].today.tm_year, data[mini].today.tm_mon, data[mini].today.tm_mday);

    free(data);
}

void menu(userdat* data, const char* id)
{
    while (1)
    {
        int op;
        printf("=======================================\n");
        printf("* 탄소 발자국 추적 프로그램 *\n");
        printf("=======================================\n");
        printf("1) 데이터 입력\n");
        printf("2) 요약 보기\n");
        printf("3) 종료\n");
        printf("=======================================\n");
        printf("선택할 메뉴 번호를 입력하세요 -> ");
        scanf_s("%d", &op);
        printf("\n");
        if (op == 1)
        {
            today_emmission(data, id);
            printf("\n");
            printf("엔터 키를 누르면 메인 메뉴로 돌아갑니다...\n");

            while (getchar() != '\n'); // 입력 버퍼 비우기

            if (getchar() == '\n')
            {
                continue;
            }

        }
        else if (op == 2)
        {
            cumulative_statistics(id);
            printf("\n");
            printf("엔터 키를 누르면 메인 메뉴로 돌아갑니다...\n");

            while (getchar() != '\n'); // 입력 버퍼 비우기 

            if (getchar() == '\n')
            {
                continue;
            }
        }
        else
        {
            break;
        }

    }
}


void login(userinf* user)
{
    printf("아이디: ");
    scanf("%10s", user->id);
    printf("비밀번호: ");
    scanf("%20s", user->password);
}

int login_process(userinf* user)
{
    userinf userdata;
    FILE* fp = fopen("userdat.dat", "rb+");
    if (fp == NULL)
    {
        return-1;
    }
    while (fread(&userdata, sizeof(userdata), 1, fp) == 1)
    {
        if (strcmp(userdata.id, user->id) == 0 && strcmp(userdata.password, user->password) == 0)
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return -1;
}

void signin()
{
    userinf user;
    FILE* fp = fopen("userdat.dat", "ab+");
    printf("새아이디: ");
    scanf("%10s", user.id);
    printf("새비밀번호: ");
    scanf("%20s", user.password);
    fwrite(&user, sizeof(userinf), 1, fp);
    fclose(fp);
}

void first_window(userdat* data, userinf* user)
{

    while (1)
    {
        int op;
        printf("1) 로그인\n");
        printf("2) 회원가입\n");
        printf("3) 종료\n");
        printf("->");
        scanf("%d", &op);
        printf("\n");
        if (op == 1)
        {
            login(user);
            if (login_process(user) == 1)
            {
                printf("로그인 성공\n");
                menu(data, user->id);
                break;
            }
            else if (login_process(user) == -1)
            {
                printf("로그인 실패\n");
            }
        }
        else if (op == 2)
        {
            signin();
            printf("\n");
            printf("엔터 키를 누르면 메인 메뉴로 돌아갑니다...\n");

            while (getchar() != '\n'); // 입력 버퍼 비우기

            if (getchar() == '\n')
            {
                continue;
            }

        }
        else
        {
            break;
        }
    }
}