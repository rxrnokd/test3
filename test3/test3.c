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
            perror("���� ���� ����");
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
        perror("������ ���� ����");
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
        printf("���� ���� ����\n");
        return NULL;
    }
    fread(&cnt, sizeof(int), 1, fp);

    userdat* data = (userdat*)malloc(sizeof(userdat) * cnt);

    if (data == NULL) {
        printf("�޸� �Ҵ� ����\n");
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
        printf("���: ź�� ���ⷮ�� 1000kg�� �ʰ��߽��ϴ�!\n");
    }
    else if (data->total > 500)
    {
        printf("����: ź�� ���ⷮ�� 500kg�� �ʰ��߽��ϴ�!\n");
    }
}

void Total_carbon_emissions(userdat* data)
{
    double car_emission = 0.2 * data->car;
    double elec_emission = 0.424 * data->elec;
    double disposable_emission = 0.1 * data->disposable;

    data->total = car_emission + elec_emission + disposable_emission;

    printf("�ڵ���: %d km x 0.2 = %.2f kg\n", data->car, car_emission);
    printf("����: %d kWh x 0.424 = %.2f kg\n", data->elec, elec_emission);
    printf("��ȸ��ǰ: %d �� x 0.1 = %.2f kg\n", data->disposable, disposable_emission);
    printf("\n");
    printf("�� ���ⷮ %.2f + %.2f + %.2f = %.2f kg CO2\n", car_emission, elec_emission, disposable_emission, data->total);
}

void today_emmission(userdat* data, const char* id)
{
    printf("----������ ź�� ���ڱ� �Է�----\n");
    printf("���� �ڵ��� �̵��Ÿ�(km): ");
    scanf_s("%d", &(data->car));
    printf("���� ���� ��뷮(kWh): ");
    scanf_s("%d", &(data->elec));
    printf("���� ��ȸ��ǰ ��� ����: ");
    scanf_s("%d", &(data->disposable));
    printf("\n");
    printf("�Է� �Ϸ�!\n");
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
        printf("�����͸� �ҷ��� �� �����ϴ�.\n");
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

    printf("���ݱ��� �� ź�� ���ⷮ: %.1f kg CO2\n", sum);
    printf("����� ź�� ���ⷮ: %.1f kg CO2\n", sum / cnt);
    printf("���ݱ����� �Ϸ� �ִ� ���ⷮ: %.1f kg CO2 %d�� %d�� %d��\n", data[maxi].total, data[maxi].today.tm_year, data[maxi].today.tm_mon, data[maxi].today.tm_mday);
    printf("���ݱ����� �Ϸ� �ּ� ���ⷮ: %.1f kg CO2 %d�� %d�� %d��\n", data[mini].total, data[mini].today.tm_year, data[mini].today.tm_mon, data[mini].today.tm_mday);

    free(data);
}

void menu(userdat* data, const char* id)
{
    while (1)
    {
        int op;
        printf("=======================================\n");
        printf("* ź�� ���ڱ� ���� ���α׷� *\n");
        printf("=======================================\n");
        printf("1) ������ �Է�\n");
        printf("2) ��� ����\n");
        printf("3) ����\n");
        printf("=======================================\n");
        printf("������ �޴� ��ȣ�� �Է��ϼ��� -> ");
        scanf_s("%d", &op);
        printf("\n");
        if (op == 1)
        {
            today_emmission(data, id);
            printf("\n");
            printf("���� Ű�� ������ ���� �޴��� ���ư��ϴ�...\n");

            while (getchar() != '\n'); // �Է� ���� ����

            if (getchar() == '\n')
            {
                continue;
            }

        }
        else if (op == 2)
        {
            cumulative_statistics(id);
            printf("\n");
            printf("���� Ű�� ������ ���� �޴��� ���ư��ϴ�...\n");

            while (getchar() != '\n'); // �Է� ���� ���� 

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
    printf("���̵�: ");
    scanf("%10s", user->id);
    printf("��й�ȣ: ");
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
    printf("�����̵�: ");
    scanf("%10s", user.id);
    printf("����й�ȣ: ");
    scanf("%20s", user.password);
    fwrite(&user, sizeof(userinf), 1, fp);
    fclose(fp);
}

void first_window(userdat* data, userinf* user)
{

    while (1)
    {
        int op;
        printf("1) �α���\n");
        printf("2) ȸ������\n");
        printf("3) ����\n");
        printf("->");
        scanf("%d", &op);
        printf("\n");
        if (op == 1)
        {
            login(user);
            if (login_process(user) == 1)
            {
                printf("�α��� ����\n");
                menu(data, user->id);
                break;
            }
            else if (login_process(user) == -1)
            {
                printf("�α��� ����\n");
            }
        }
        else if (op == 2)
        {
            signin();
            printf("\n");
            printf("���� Ű�� ������ ���� �޴��� ���ư��ϴ�...\n");

            while (getchar() != '\n'); // �Է� ���� ����

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