#ifndef __QUOTADB_H__
#define __QUOTADB_H__

#include <stdio.h>
#include <stdlib.h>
#include "postgresql/libpq-fe.h"
#include <time.h>
// #include "globals.h"
#include "SquidTime.h"
#include "UserInfo.h"


#define HOST "localhost"
#define USER "squid2"
#define PASS "squid"
#define DBNAME "quotasDB"
#define TABLE "quotas"


class QuotaDB
{
public:
    QuotaDB()
    {
        
    }
    ~QuotaDB()
    {
        sprintf(query, "dbname=%s host=%s user=%s password=%s", DBNAME, HOST, USER, PASS);
        conn = PQconnectdb(query);
        if (PQstatus(conn) == CONNECTION_BAD) {
            printf("Unable to connect to the database");
            return 0;
        }
    }

    // float saveSize(const char *user, float mb_size, time_t curr_time);
    void SaveData(const char *username, float current, const char *mod_time);
    void Find(const char *username, UserInfo* userInfo);
    int Quota(const char *username);
    float Consumed(const char *username);

private:
    PGconn *conn;
    PGresult *res;
    // int rec_count;
    // int row;
    // int col;
    char query[1024];
};




#endif