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
        char* id = (char*)malloc(17);
        if (id == NULL) {
            return MEM_ALLOC_ERR;
        }
        scanf("%s", id);
        string s;

        if (!string_create(id, &s)) {
            return MEM_ALLOC_ERR;
        }
        if (!ValidatePostId(s)) {
            printf("Некорректный идентификатор\n");
            return INC_INP_DATA;
        }

        Mail* m = FindByPostID(s, post);
        if (m == NULL) {
            printf("Посылки с таким идентификатором нет\n");
        }
        else {
            PrintMail(m);
        }
        string_destroy(&s);
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
        if (!string_create(id_d, &s_d)) {
            return MEM_ALLOC_ERR;
        }
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
        string_destroy(&s_d);
    case 'h':
        PrintHelp();
        break;
    case 'v':
        if (post->mail_amount == 0) {
            printf("Нет добавленных посылок\n");
        }
        for (int i = 0; i < post->mail_amount; i++) {
            PrintMail(&(post->mails[i]));
        }
        break;
    case 't':
        time_t mytime = time(NULL);
        struct tm* now = localtime(&mytime);
        struct tm cur;
        if (post->mail_amount == 0) {
            printf("Нет добавленных посылок\n");
            break;
        }
        string t;
        if (!string_create("-1", &t)) {
            return MEM_ALLOC_ERR;
        }

        qsort(post->mails, post->mail_amount, sizeof(Mail), MailComparatorByCreationTime);
        for (int i = 0; i < post->mail_amount; i++) {
            if (string_comp(&(post->mails[i].post_id), &t) != 0) {
                cur = ConvertToTm(post->mails[i].time_get);

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
        break;
    case 's':
        printf("Остановка программы\n");
        break;
    default:
        return INC_FLAG;
    }
    return SUCCESS;
}

void ClearBuffer(void)
{
    char c;
    while (getchar() != '\n' && getchar != EOF);
}

int main()
{

    int nd, nM, ny, nh, nm, ns;

    const int start_buf_size = 10;
    setlocale(LC_ALL, "RU");
    Post post;

    string city;
    string street;
    string post_id;
    string building;

    if (!string_create("Moscow", &city)) {
        return ProccessError(MEM_ALLOC_ERR);
    }
    if (!string_create("Volokolamskoe shosse", &street)) {
        string_destroy(&city);
        return ProccessError(MEM_ALLOC_ERR);
    }
    if (!string_create("112547", &post_id)) {
        string_destroy(&city);
        string_destroy(&street);
        return ProccessError(MEM_ALLOC_ERR);
    }
    if (!string_create("", &building)) {
        string_destroy(&city);
        string_destroy(&street);
        string_destroy(&post_id);
        return ProccessError(MEM_ALLOC_ERR);
    }
    



    bool res = CreateAddress(city, street, 4, building, 
                    18, post_id, &(post.post_address));
    if (!res) {
        return INC_ARGS;
    }
    
    post.mail_amount = 0;
    post.mail_buf_size = start_buf_size;
    post.mails = (Mail*)malloc(sizeof(Mail) * start_buf_size);

    /*string s = string_create("-1");
    for (int i = 0; i < start_buf_size; i++) {
        post.mails[i].post_id = s;
    }*/

    if (post.mails == NULL) {
        return MEM_ALLOC_ERR;
    }


    printf("Почтовое отделение расположено по адресу:");
    PrintAddress(&(post.post_address));
    printf("\n");
    char c = '0';
    char cn;
    while (!(c == 's' && cn == '\n')) {
        int count = fscanf(stdin, "%c", &c);
        count += fscanf(stdin, "%c", &cn);
        if (cn != '\n') {
            printf("Некорректный формат ввода\n");
            ClearBuffer();
            continue;
        }
        if (count != 2) {
            printf("Неккоректный флаг\n");
            ClearBuffer();
        }

        kErrors status = ProccessFlag(c, &post);
        if (status != INC_INP_DATA && status != SUCCESS && status != INC_FLAG) {
            DestroyMails(post.mails, post.mail_amount);
            return ProccessError(status);
        } else {
			switch (status) {
				case INC_FLAG:
					printf("Incorrect flag\n");
					break;
			}
		}
        fseek(stdin, 0, SEEK_END);
	}
    string_destroy(&city);
    string_destroy(&street);
    string_destroy(&post_id);
    string_destroy(&building);
    DestroyMails(post.mails, post.mail_amount);
    return 0;
}

