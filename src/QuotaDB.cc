#include "squid.h"
#include <stdio.h>
#include <stdlib.h>
#include "postgresql/libpq-fe.h"
#include <time.h>
// #include "globals.h"
// #include "SquidTime.h"
// #include "UserInfo.h"

#include "QuotaDB.h"

void QuotaDB::SaveData(const char *username, long long int current) {
    sprintf(query, "dbname=%s host=%s user=%s password=%s", DBNAME, HOST, USER, PASS);
    conn = PQconnectdb(query);

    if(PQstatus(conn) == CONNECTION_OK)
    {
        sprintf(query, "UPDATE %s SET consumido=%lld WHERE correo='%s'", TABLE, current, username);
        res = PQexec(conn,query);
        PQclear(res);
    } else {
        debugs(33, DBG_CRITICAL, "" << PQerrorMessage(conn));
    }
    PQfinish(conn);
}

UserInfo*
QuotaDB::Find(const char *username) {
    sprintf(query, "dbname=%s host=%s user=%s password=%s", DBNAME, HOST, USER, PASS);
    conn = PQconnectdb(query);
    UserInfo* user = NULL;
    if(PQstatus(conn) == CONNECTION_OK)
    {
        sprintf(query, "SELECT cuota_internet,consumido FROM %s WHERE correo='%s'", TABLE, username);
        debugs(33, DBG_IMPORTANT, "Inside Find User");
        res = PQexec(conn,query);
        debugs(33, DBG_IMPORTANT, "ResStatus: " << PQresultStatus(res));
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            debugs(33, DBG_IMPORTANT, "Inside If to create user");
            debugs(33, DBG_IMPORTANT, "Find----" << PQgetvalue(res,0,1));
            user = new UserInfo(username, atoi(PQgetvalue(res, 0, 0)), atoll(PQgetvalue(res, 0, 1)), 19, squid_curtime);
            debugs(33, DBG_IMPORTANT, "After");
        }
        else {
            debugs(33, DBG_CRITICAL, "Error " << PQresultErrorMessage(res));
        }
        debugs(33, DBG_IMPORTANT, "By Find User");
        PQclear(res);
    } else {
        debugs(33, DBG_CRITICAL, "" << PQerrorMessage(conn));
    }
    PQfinish(conn);
    return user;
} 
int
QuotaDB::Quota(const char *username) {
    sprintf(query, "dbname=%s host=%s user=%s password=%s", DBNAME, HOST, USER, PASS);
    conn = PQconnectdb(query);

    if(PQstatus(conn) == CONNECTION_OK)
    {
        sprintf(query, "SELECT cuota_internet FROM %s WHERE correo='%s'", TABLE, username);
        int quota = 0;
        res = PQexec(conn,query);
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            quota = atoi(PQgetvalue(res, 0, 0));
        }
        else {
            printf("Error: %s",PQresultErrorMessage(res));
        }
        PQclear(res);
        PQfinish(conn);
        return quota;
    } else {
        debugs(33, DBG_CRITICAL, "" << PQerrorMessage(conn));
    }
    PQfinish(conn);
    return 0;
}
long long int
QuotaDB::Consumed(const char *username) {
    sprintf(query, "dbname=%s host=%s user=%s password=%s", DBNAME, HOST, USER, PASS);
    conn = PQconnectdb(query);

    if(PQstatus(conn) == CONNECTION_OK)
    {
        sprintf(query, "SELECT consumido FROM %s WHERE correo='%s'", TABLE, username);
        int consumed = 0;
        res = PQexec(conn,query);
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            consumed= atoll(PQgetvalue(res, 0, 0));
        }
        else {
            printf("Error: %s",PQresultErrorMessage(res));
        }
        PQclear(res);
        PQfinish(conn);
        return consumed;
    } else {
        debugs(33, DBG_CRITICAL, "" << PQerrorMessage(conn));
    }
    PQfinish(conn);
    return 0;
}
