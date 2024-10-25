#define _CRT_SECURE_NO_WARNINGS
#define _XOPEN_SOURCE
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "locale.h"
#include "limits.h"
#include "stdarg.h"
#include "stdint.h"
#include "stdio.h"
#include "myerrors.h"
#include "string.h"
#include "mystring.h"
#include "mail.h"
#include <time.h>

//
//typedef enum kMailErrors{
//    INC_FLAG,
//    INC_POST_ID
//}kMailErrors;

void PrintHelp() {
    printf("Программа распознает следующие флаги:\n");
    printf("g - поиск посылки с определенным почтовым идентификатором\n");
    printf("a - добавленеи посылки\n");
    printf("d - удаление посылки по идентификатору\n");
    printf("v - просмотр всех сохраненных посылок\n");
    printf("t - вывод доставленных/недоставленных посылок\n");
}

kErrors ProccessFlag(char flag, Post* post) {
    switch (flag) {
    case 'g':
        printf("Введите идентификатор:\n");
        char* id = (char*)malloc(16);
        if (id == NULL) {
            return MEM_ALLOC_ERR;
        }
        scanf("%s", id);
        string s;
        s = string_create(id);
        if (!ValidatePostId(s)) {
            printf("Некорректный идентификатор\n");
            free(id);
            return INC_INP_DATA;
        }

        Mail* m = FindByPostID(s, post);
        if (m == NULL) {
            printf("Посылки с таким идентификатором нет\n");
        }
        else {
            PrintMail(m);
        }
        free(id);
        break;
    case 'a':
        printf("Введите данные посылки:\n");
        Mail m_a;
        if (!ScanMail(&m_a)) {
            printf("Некорректные данные\n");
            return INC_INP_DATA;
        }
        if (!AddMail(post, m_a)) {
            return MEM_ALLOC_ERR;
        }
        printf("Посылка успешно добавлена\n");
        break;
    case 'd':
        printf("Введите идентификатор:\n");
        char* id_d = (char*)malloc(16);
        if (id_d == NULL) {
            return MEM_ALLOC_ERR;
        }
        scanf("%s", id_d);
        string s_d;
        s_d = string_create(id_d);
        if (!ValidatePostId(s_d)) {
            printf("Некорректный идентификатор\n");
            free(id_d);
            return INC_INP_DATA;
        }
        if (!DeleteMail(post, s_d)) {
            printf("Такой посылки нет в списке\n");
        }
        else {
            printf("Посылка успешно удалена\n");
        }
        free(id_d);
        break;
    case 'h':
        PrintHelp();
        break;
    case 'v':
        if (post->mail_amount == 0) {
            printf("Нет добавленных посылок\n");
        }
        for (int i = 0; i < post->mail_amount; i++) {
            PrintMail(&(post->mails[i]));
            printf("\n");
        }
        break;
    case 't':
        time_t mytime = time(NULL);
        struct tm* now = localtime(&mytime);
        struct tm cur;
        string t = string_create("-1");

        qsort(post->mails, post->mail_amount, sizeof(Mail), MailComparatorByCreationTime);
        for (int i = 0; i < post->mail_amount; i++) {
            if (string_comp(&(post->mails[i].post_id), &t) != 0) {
                cur = *ConvertToTm(post->mails[i].time_get);

                PrintMail(&(post->mails[i]));                
                if (difftime(mktime(now), mktime(&cur)) > 0) {
                    printf("Посылка доставлена.\n");
                }
                else {
                    printf("Посылка не доставлена.\n");
                }
            }
        }
        qsort(post->mails, post->mail_amount, sizeof(Mail), MailComparatorBase);
    }
}


void strings_demo() {

}

int main()
{

    int nd, nM, ny, nh, nm, ns;

    const int start_buf_size = 10;
    setlocale(LC_ALL, "RU");
    Post post;
    bool res = CreateAddress(string_create("Moscow"), string_create("Volokolamskoe shosse"), 4, string_create(""), 
                    18, string_create("112547"), &(post.post_address));
    if (!res) {
        return INC_ARGS;
    }
    
    post.mail_amount = 0;
    post.mail_buf_size = start_buf_size;
    post.mails = (Mail*)malloc(sizeof(Mail) * start_buf_size);
    if (post.mails == NULL) {
        return MEM_ALLOC_ERR;
    }


    printf("Почтовое отделение расположено по адресу:");
    PrintAddress(&(post.post_address));
    printf("\n");
    char c = '0';
    while (c != 's') {
        int count = scanf("%c", &c);
        if (c == '\n') {
            continue;
        }
        if (count != 1) {
            printf("Incorrect flag\n");
        }
        ProccessFlag(c, &post);       
    }
}

