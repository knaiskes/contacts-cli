#ifndef CONTACT_H_
#define CONTACT_H_

struct Contact {
    char Name[20];
    char LastName[20];
    char PhoneNumber1[20];
    char PhoneNumber2[20];
    char Email[30];
    char Address[30];
};

void addNewContact();

#endif // CONTACT_H_
