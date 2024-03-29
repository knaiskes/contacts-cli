#include <stdio.h>
#include <sqlite3.h>

#include "contact.h"

static sqlite3 *db;
static int *zErrMsg = 0;
static int rc;

char *dbName = "contacts.db";

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    for(int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void checkSQLiteOK(int rc, int zErrMsg, char *successMsg, char *failMsg)
{
    if(rc != SQLITE_OK) {
        fprintf(stderr, "%s\n", failMsg, zErrMsg);
    }
    else {
        fprintf(stdout, "%s\n", successMsg);
    }
}

void CreateDatabase(const char *dbName)
{
    char* sql;
    rc = sqlite3_open(dbName, &db);

    sql = "CREATE TABLE IF NOT EXISTS contacts("            \
           "ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL," \
           "NAME CHAR(20) NOT NULL,"                        \
           "LASTNAME CHAR(20) NOT NULL,"                    \
           "PHONENUMBER1 CHAR(20),"                          \
           "PHONENUMBER2 CHAR(20),"                          \
           "EMAIL CHAR(30),"                                \
           "ADDRESS CHAR(30)"                               \
           ");"                                             \
           ;

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    checkSQLiteOK(rc, zErrMsg, "Database was successfully created",
            "Could not create database");

    sqlite3_close(db);
}

void InsertContact(const struct Contact* contact)
{
    char *sql = sqlite3_mprintf(
            "INSERT INTO contacts"                                       \
            "(NAME, LASTNAME, PHONENUMBER1, PHONENUMBER2, EMAIL, ADDRESS)" \
            "VALUES('%q','%q','%q','%q','%q','%q');",                    \
            contact->Name, contact->LastName, contact->PhoneNumber1,
            contact->PhoneNumber2, contact->Email, contact->Address
            );

    rc = sqlite3_open(dbName, &db);

    if(rc) {
        fprintf(stderr, "Error while inserting new contact %s\n",
                sqlite3_errmsg(db));
    }

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    checkSQLiteOK(rc, zErrMsg, "Contact was added", "Something went wrong");

    sqlite3_close(db);
}

void DeleteContact(const struct Contact* contact)
{
    char *sql = sqlite3_mprintf(
            "DELETE FROM contacts WHERE name = ('%q') AND lastname = ('%q');",
            contact->Name, contact->LastName);

    rc = sqlite3_open(dbName, &db);

    if(rc) {
        fprintf(stderr, "Error while deleting a contact %s\n",
                sqlite3_errmsg(db));
    }

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    checkSQLiteOK(rc, zErrMsg, "Contact was successfully deleted",
            "Could not delete contact");

    sqlite3_close(db);
}

int contactExists(char *name, char *lastName)
{
    int exists = 0;
    char *sql = sqlite3_mprintf(
            "SELECT id FROM contacts WHERE name = ('%q') AND lastname = ('%q');",
            name, lastName);

    sqlite3_stmt *res;
    rc = sqlite3_open(dbName, &db);

    if(rc != SQLITE_OK) {
        fprintf(stderr, "Error while checking if contact exists\n");
        sqlite3_close(db);
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    int step = sqlite3_step(res);

    if(step == SQLITE_ROW) {
        exists = 1;
    }

    sqlite3_finalize(res);
    sqlite3_close(db);

    return exists;
}

void listAllContacts()
{
    char *sql = "SELECT * FROM contacts";
    char *err_msg = 0;

    rc = sqlite3_open(dbName, &db);

    if(rc) {
        fprintf(stderr, "Error while trying to fetch all contacts %s\n",
                sqlite3_errmsg(db));
    }

    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

    sqlite3_close(db);
}

void updateContact(struct Contact* contact, char *name, char *lastname)
{
    char *sql = sqlite3_mprintf(
            "UPDATE contacts SET                        " \
            "NAME = ('%q'),                             " \
            "LASTNAME = ('%q'),                         " \
            "PHONENUMBER1 = ('%q'),                     " \
            "PHONENUMBER2 = ('%q'),                     " \
            "EMAIL = ('%q'),                            " \
            "ADDRESS = ('%q')                           " \
            "WHERE name = ('%q') AND lastname = ('%q')  " \
            ";                                          " \
            ,
            contact->Name, contact->LastName, contact->PhoneNumber1,
            contact->PhoneNumber2, contact->Email, contact->Address,
            name, lastname
            );

    rc = sqlite3_open(dbName, &db);

    if(rc) {
        fprintf(stderr, "Error while trying to update contact %s\n",
                sqlite3_errmsg(db));
    }

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    sqlite3_close(db);
}
