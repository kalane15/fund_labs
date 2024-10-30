#pragma once

#include "mystring.h"
#include "time.h"
#define ll long long
#define ull unsigned long long
#define ld long double
#define uint unsigned int

typedef struct Address {
	string city;
	string street;
	int house;
	string building;
	int apartment;
	string index;
} Address;

typedef struct Mail {
	Address address;
	string post_id;
	double weight;
	string time_created;
	string time_get;
} Mail;

typedef struct Post {
	Address post_address;
	Mail* mails;
	int mail_amount;
	int mail_buf_size;
} Post;

bool ValidateAddressData(string city, string street, int house, string building, int apartment, string index);

bool ValidateTime(string time);

bool ValidatePostId(string post_id);

bool CreateAddress(string city, string street, int house, string building, int apartment, string index, Address* out);

bool ValidateMailData(string city, string street, int house, string building, int apartment, string index, string post_id, double weight, string time_created, string time_get);

bool CreateMail(string city, string street, int house, string building, int apartment, string index, string post_id, double weight, string time_created, string time_get, Mail* out);

bool AddMail(Post* post, Mail mail);

void PrintAddress(Address* adr);

Mail* FindByPostID(string post_id, Post* post);

void PrintMail(Mail* m);

bool ScanMail(Mail* m);
struct tm ConvertToTm(string time);
bool DeleteMail(Post* post, string post_id);
int MailComparatorByCreationTime(void* a, void* b);
int MailComparatorBase(void* a, void* b);
void DestroyMails(Mail* mails, int size);