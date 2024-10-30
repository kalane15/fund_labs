#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "time.h"

#include "limits.h"
#include "stdarg.h"
#include "stdint.h"
#include "stdio.h"
#include "myerrors.h"
#include "string.h"
#include "mystring.h"
#include "mail.h"

#define BUFFER_SIZE 1024

bool ValidateAddressData(string city, string street, int house, string building, int apartment, string index) {
	if (city.size == 0 || street.size == 0 || index.size != 6) {
		return false;
	}
	if (house <= 0 || apartment <= 0) {
		return false;
	}


	for (int i = 0; i < index.size; i++) {
		if (!('0' <= index.symbols[i] && index.symbols[i] <= '9')) {
			return false;
		}
	}
	return true;
}

bool ValidateTime(string time) {
	int d, M, y, h, m, s;
	int nd, nM, ny, nh, nm, ns;

	sscanf(time.symbols, "%*[0-9]%n:%*[0-9]%n:%*[0-9]%n %*[0-9]%n:%*[0-9]%n:%*[0-9]%n", &nd, &nM, &ny, &nh, &nm, &ns);

	if (nd != 2 || nm != 16 || ny != 10 || nh != 13 || nM != 5 || ns != 19) {
		return false;
	}


	int count;
	

	count = sscanf(time.symbols, "%d:%d:%d %d:%d:%d", &d, &M, &y, &h, &m, &s);
	if (count != 6) {
		return false;
	}

	if (d < 0 || d > 31 || M < 1 || M > 12 || y <= 0 || h < 0 || h > 23 || m < 0 || m > 59 || s < 0 || s > 59) {
		return false; 
	}
	return true;
}

bool ValidatePostId(string post_id) {
	if (post_id.size != 14) {
		return false;
	}

	for (int i = 0; i < post_id.size; i++) {
		if (!('0' <= post_id.symbols[i] && post_id.symbols[i] <= '9')) {
			return false;
		}
	}
}

bool CreateAddress(string city, string street, int house, string building, int apartment, string index, Address* out) {
	if (!ValidateAddressData(city, street, house, building, apartment, index)) {
		return false;
	}
	out->city = city;
	out->street = street;
	out->house = house;
	out->building = building;
	out->index = index;
	out->apartment = apartment;
	return true;
}

bool ValidateMailData(string city, string street, int house, string building, int apartment, string index,
	string post_id, double weight, string time_created, string time_get) {
	bool res = ValidateAddressData(city, street, house, building, apartment, index);
	if (!res) {
		return false;
	}

	if (weight <= 0.0) {
		return false;
	}

	if (post_id.size != 14) {
		return false;
	}

	for (int i = 0; i < post_id.size; i++) {
		if (!('0' <= post_id.symbols[i] && post_id.symbols[i] <= '9')) {
			return false;
		}
	}

	if (!ValidateTime(time_created)) {
		return false;
	}

	if (!ValidateTime(time_get)) {
		return false;
	}

	struct tm t1 = ConvertToTm(time_created);
	struct tm t2 = ConvertToTm(time_get);
	if (difftime(mktime(&t1), mktime(&t2)) > 0) {
		return false;
	}
	return true;
}

bool CreateMail(string city, string street, int house, string building, int apartment, string index,
	string post_id, double weight, string time_created, string time_get, Mail* out) {
	if (!ValidateMailData(city, street, house, building, apartment, index, post_id, weight, time_created, time_get)) {
		return false;
	}

	Address adr;
	if (!CreateAddress(city, street, house, building, apartment, index, &adr)) {
		return false;
	}

	out->address = adr;
	out->post_id = post_id;
	out->time_created = time_created;
	out->time_get = time_get;
	out->weight = weight;
	return true;
}

int MailComparatorBase(void* a, void* b) {
	Mail* m1 = (Mail*)a;
	Mail* m2 = (Mail*)b;
	if (string_comp(&(m1->address.index), &(m2->address.index)) != 0) {
		return string_comp(&(m1->address.index), &(m2->address.index));
	}
	else {
		return string_comp(&(m1->post_id), &(m2->post_id));
	}
}

int MailComparatorByCreationTime(void* a, void* b) {
	Mail* m1 = (Mail*)a;
	Mail* m2 = (Mail*)b;
	struct tm t1 = ConvertToTm(m1->time_created);
	struct tm t2 = ConvertToTm(m2->time_created);

	return difftime(mktime(&t1), mktime(&t2)) > 0 ? 1 : -1;
}

bool AddMail(Post* post, Mail mail) {
	if (post->mail_buf_size == post->mail_amount) {
		Mail* temp = post->mails;
		
		post->mails = (Mail*)realloc(post->mails, sizeof(Mail) * post->mail_buf_size * 2);
		if (post->mails == NULL) {
			post->mails = temp;
			return false;
		}

		/*string s = string_create("-1");
		for (int i = post->mail_buf_size; i < post->mail_buf_size; i++) {
			post->mails[i].post_id = s;
		}*/

		post->mail_buf_size *= 2;
	}
	
	post->mails[post->mail_amount++] = mail;	
	qsort(post->mails, post->mail_amount, sizeof(Mail), MailComparatorBase);
	return true;
}

void DestroyMail(Mail* m) {
	string_destroy(&m->post_id);
	string_destroy(&m->time_created);
	string_destroy(&m->time_get);
	string_destroy(&m->address.building);
	string_destroy(&m->address.city);
	string_destroy(&m->address.index);
	string_destroy(&m->address.street);
}

bool DeleteMail(Post* post, string post_id) {
	Mail* m = FindByPostID(post_id, post);
	if (m == NULL) {
		return false;
	}
	
	int i;
	for (i = 0; i < post->mail_amount; i++) {
		if (string_comp(&(post->mails[i].post_id), &post_id) == 0) {
			DestroyMail(&(post->mails[i]));
			break;
		}
	}
	post->mail_amount--;
	for (; i < post->mail_amount; i++) {
		post->mails[i] = post->mails[i + 1];
	}
	return true;
}

void PrintAddress(Address* adr) {
	printf("город  %s, улица %s, дом %d %s, квартира %d, почтовый индекс: %s",
		string_pr(adr->city), string_pr(adr->street), adr->house, string_pr(adr->building), adr->apartment, string_pr(adr->index));
}

Mail* FindByPostID(string post_id, Post* post) {
	for (int i = 0; i < post->mail_amount; i++) {
		if (string_comp(&(post->mails[i].post_id), &post_id) == 0) {
			return &(post->mails[i]);
		}
	}
	return NULL;
}

void PrintMail(Mail* m) {
	printf("Посылка весом %lf с почтовым идентификатором: %s. Время создания:%s. Время вручения:%s.", 
		m->weight, string_pr(m->post_id), string_pr(m->time_created), string_pr(m->time_get));
	printf("Адрес получателя:");
	PrintAddress(&(m->address));
	printf("\n");
}



bool ScanMail(Mail* m) {
	char* city_s = (char*)malloc(BUFFER_SIZE);
	char* street_s = (char*)malloc(BUFFER_SIZE);
	char* building_s = (char*)malloc(BUFFER_SIZE);
	char* index_s = (char*)malloc(BUFFER_SIZE);
	char* post_id_s = (char*)malloc(BUFFER_SIZE);
	char* time_created_s = (char*)malloc(BUFFER_SIZE);
	char* time_get_s = (char*)malloc(BUFFER_SIZE);
	char* temp = (char*)malloc(BUFFER_SIZE);


	if (!city_s || !street_s || !building_s || !index_s || !post_id_s || !time_created_s || !temp) {
		if (city_s) {
			free(city_s);
		}
		if (street_s) {
			free(street_s);
		}
		if (building_s) {
			free(building_s);
		}
		if (index_s) {
			free(index_s);
		}
		if (post_id_s) {
			free(post_id_s);
		}
		if (time_created_s) {
			free(time_created_s);
		}
		if (temp) {
			free(temp);
		}
	}
	bool status = true;
	strcpy(city_s, "");
	strcpy(street_s, "");
	strcpy(building_s, "");
	strcpy(index_s, "");
	strcpy(post_id_s, "");
	strcpy(time_created_s, "");
	strcpy(time_get_s, "");

	double weight;
	int apartment;
	int house;
	int count = 0;
	char* a;
	printf("Введите город:\n");
	a = gets(city_s);
	if (a == NULL) {
		free(city_s);
		free(street_s);
		free(building_s);
		free(index_s);
		free(post_id_s);
		free(time_created_s);
		free(time_get_s);
		free(temp);
		return false;
	}
	printf("Введите улицу:\n");
	a = gets(street_s);
	if (a == NULL) {
		free(city_s);
		free(street_s);
		free(building_s);
		free(index_s);
		free(post_id_s);
		free(time_created_s);
		free(time_get_s);
		free(temp);
		return false;
	}
	printf("Введите номер дома:\n");
	count = scanf("%d", &house);
	if (count != 1) {
		free(city_s);
		free(street_s);
		free(building_s);
		free(index_s);
		free(post_id_s);
		free(time_created_s);
		free(time_get_s);
		free(temp);
		return false;
	}
	getchar();
	printf("Введите адрес строения:\n");
	a = gets(building_s);
	if (a == NULL) {
		free(city_s);
		free(street_s);
		free(building_s);
		free(index_s);
		free(post_id_s);
		free(time_created_s);
		free(time_get_s);
		free(temp);
		return false;
	}
	printf("Введите номер квартиры:\n");
	count = scanf("%d", &apartment);
	if (count != 1) {
		free(city_s);
		free(street_s);
		free(building_s);
		free(index_s);
		free(post_id_s);
		free(time_created_s);
		free(time_get_s);
		free(temp);
		return false;
	}
	printf("Введите почтовый индекс:\n");
	count = scanf("%s", index_s);
	if (count != 1) {
		free(city_s);
		free(street_s);
		free(building_s);
		free(index_s);
		free(post_id_s);
		free(time_created_s);
		free(time_get_s);
		free(temp);
		return false;
	}
	printf("Введите почтовый идентификатор:\n");
	count = scanf("%s", post_id_s);
	if (count != 1) {
		free(city_s);
		free(street_s);
		free(building_s);
		free(index_s);
		free(post_id_s);
		free(time_created_s);
		free(time_get_s);
		free(temp);
		return false;
	}
	printf("Введите вес посылки:\n");
	count = scanf("%lf", &weight);
	if (count != 1) {
		free(city_s);
		free(street_s);
		free(building_s);
		free(index_s);
		free(post_id_s);
		free(time_created_s);
		free(time_get_s);
		free(temp);
		return false;
	}

	printf("Введите время создания:\n");
	count = scanf("%s %s", time_created_s, temp);
	if (count != 2) {
		free(city_s);
		free(street_s);
		free(building_s);
		free(index_s);
		free(post_id_s);
		free(time_created_s);
		free(time_get_s);
		free(temp);
		return false;
	}

	strcat(time_created_s, " ");
	strcat(time_created_s, temp);
	printf("Введите время вручения:\n");
	count = scanf("%s %s", time_get_s, temp);
	if (count != 2) {
		free(city_s);
		free(street_s);
		free(building_s);
		free(index_s);
		free(post_id_s);
		free(time_created_s);
		free(time_get_s);
		free(temp);
		return false;
	}

	strcat(time_get_s, " ");
	strcat(time_get_s, temp);
	string city; 
	string_create(city_s, &city);
	string street;
	string_create(street_s, &street);
	string building;
	string_create(building_s, &building);
	string index;
	string_create(index_s, &index);
	string post_id;
	string_create(post_id_s, &post_id);
	string time_created;
	string_create(time_created_s, &time_created);
	string time_get;
	string_create(time_get_s, &time_get);


	free(city_s);
	free(street_s);
	free(building_s);
	free(index_s);
	free(post_id_s);
	free(time_created_s);
	free(time_get_s);
	free(temp);
	if (status != true) {
		return false;
	}
	status = CreateMail(city, street, house, building, apartment, index, post_id, weight, time_created, time_get, m);	
	return status;
}

struct tm ConvertToTm(string time) {
	int d, M, y, h, m, s;
	struct tm out;
	sscanf(time.symbols, "%d:%d:%d %d:%d:%d", &d, &M, &y, &h, &m, &s);
	out.tm_mday = d;	
	out.tm_mon = M - 1;
	out.tm_year = y - 1900;
	out.tm_hour = h;
	out.tm_min = m;
	out.tm_sec = s;
	out.tm_isdst = -1;
	return out;
}

void DestroyMails(Mail* mails, int size) {
	for (int i = 0; i < size; i++) {
		string_destroy(&mails[i].post_id);
		string_destroy(&mails[i].time_created);
		string_destroy(&mails[i].time_get);
		string_destroy(&mails[i].address.building);
		string_destroy(&mails[i].address.city);
		string_destroy(&mails[i].address.index);
		string_destroy(&mails[i].address.street);
	}
	free(mails);
}