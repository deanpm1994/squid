#ifndef __QUOTADB_H__
#define __QUOTADB_H__

#include <stdio.h>
#include <stdlib.h>
#include "postgresql/libpq-fe.h"
#include <time.h>
// #include "globals.h"
#include "SquidTime.h"
#include "UserInfo.h"
#include "Debug.h"
#include "UserInfo.h"


#define HOST "10.6.143.9"
#define USER "proxy"
#define PASS "yoidUct0"
#define DBNAME "proxy_db"
#define TABLE "estudiantes_dean"


class QuotaDB
{
public:

    // float saveSize(const char *user, float mb_size, time_t curr_time);
    void SaveData(const char *username, long long int current);
    void DeleteUser(const char *username);
    UserInfo* Find(const char *username);
    int Quota(const char *username);
    void UpdateQuota(const char *username, int quota);
    void ResetConsumed(const char *username);
    bool Overquota(const char *username);
    QuotaDB()
    {
        sprintf(query, "dbname=%s host=%s user=%s password=%s", DBNAME, HOST, USER, PASS);
        conn = PQconnectdb(query);
        if (PQstatus(conn) != CONNECTION_OK) {
            debugs(33, DBG_CRITICAL, "Unable to connect to the database");
            debugs(33, DBG_CRITICAL, "" << PQerrorMessage(conn));
            PQfinish(conn);
        }
    }
    ~QuotaDB()
    {
        PQfinish(conn);
    }

private:
    PGconn *conn;
    PGresult *res;
    // int rec_count;
    // int row;
    // int col;
    char query[1024];
};




#endif