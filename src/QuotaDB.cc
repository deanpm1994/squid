#include "squid.h"
#include <stdio.h>
#include <stdlib.h>
#include "postgresql/libpq-fe.h"
#include "Mem.h"
#include <time.h>
// #include "globals.h"
// #include "SquidTime.h"
// #include "UserInfo.h"

#include "QuotaDB.h"

void 
QuotaDB::SaveData(const char *username, long long int current) {
    // sprintf(query, "dbname=%s host=%s user=%s password=%s", DBNAME, HOST, USER, PASS);
    // conn = PQconnectdb(query);

    // if(PQstatus(conn) == CONNECTION_OK)
    // {
    sprintf(query, "UPDATE %s SET consumido=%lld WHERE correo='%s'", TABLE, current, username);
    res = PQexec(conn,query);
    PQclear(res);
    // } else {
    //     debugs(33, DBG_CRITICAL, "" << PQerrorMessage(conn));
    // }
    // PQfinish(conn);
}
void 
QuotaDB::DeleteUser(const char *username) {
    // sprintf(query, "dbname=%s host=%s user=%s password=%s", DBNAME, HOST, USER, PASS);
    // conn = PQconnectdb(query);

    // if(PQstatus(conn) == CONNECTION_OK)
    // {
    sprintf(query, "DELETE FROM %s WHERE correo='%s'", TABLE, username);
    res = PQexec(conn,query);
    PQclear(res);
    // } else {
    //     debugs(33, DBG_CRITICAL, "" << PQerrorMessage(conn));
    // }
    // PQfinish(conn);
}

UserInfo*
QuotaDB::Find(const char *username) {
    // sprintf(query, "dbname=%s host=%s user=%s password=%s", DBNAME, HOST, USER, PASS);
    // conn = PQconnectdb(query);
    UserInfo* user = (UserInfo *)memAllocate(MEM_CLIENT_INFO);
    // if(PQstatus(conn) == CONNECTION_OK)
    // {
    sprintf(query, "SELECT cuota_internet,consumido FROM %s WHERE correo='%s'", TABLE, username);
    debugs(33, DBG_IMPORTANT, "Inside Find User");
    res = PQexec(conn,query);
    debugs(33, DBG_IMPORTANT, "ResStatus: " << PQresultStatus(res));
    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        debugs(33, DBG_IMPORTANT, "Inside If to create user");
        debugs(33, DBG_IMPORTANT, "Find----" << PQgetvalue(res,0,1));
        // user = (UserInfo *)memAllocate(MEM_CLIENT_INFO);
        user->hash.key = xstrdup(username);
        user->username = username;
        user->quota = atoi(PQgetvalue(res, 0, 0));
        user->current = atoll(PQgetvalue(res, 0, 1));
        user->tunnel = 19;
        user->expiretime = squid_curtime;
        // user = new UserInfo(username, atoi(PQgetvalue(res, 0, 0)), atoll(PQgetvalue(res, 0, 1)), 19, squid_curtime);
        debugs(33, DBG_IMPORTANT, "After");
    }
    else {
        debugs(33, DBG_CRITICAL, "Error " << PQresultErrorMessage(res));
    }
    debugs(33, DBG_IMPORTANT, "By Find User");
    PQclear(res);
    // } else {
    //     debugs(33, DBG_CRITICAL, "" << PQerrorMessage(conn));
    // }
    // PQfinish(conn);
    return user;
} 
int
QuotaDB::Quota(const char *username) {
    debugs(33, DBG_IMPORTANT, "In QuotaDBQuota");
    // sprintf(query, "dbname=%s host=%s user=%s password=%s", DBNAME, HOST, USER, PASS);
    // conn = PQconnectdb(query);

    // if(PQstatus(conn) == CONNECTION_OK)
    // {
    debugs(33, DBG_IMPORTANT, "conn is ok");
    sprintf(query, "SELECT cuota_internet FROM %s WHERE correo='%s'", TABLE, username);
    int quota = 0;
    res = PQexec(conn,query);
    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
    debugs(33, DBG_IMPORTANT, "query is ok");
        quota = atoi(PQgetvalue(res, 0, 0));
        debugs(33, DBG_IMPORTANT, "this is quota " << (int)quota);
    }
    else {
    debugs(33, DBG_CRITICAL, "bad query" << PQresultErrorMessage(res));
        printf("Error: %s",PQresultErrorMessage(res));
    }
    PQclear(res);
    // PQfinish(conn);
    debugs(33, DBG_IMPORTANT, "before return");
    return quota;
    // } else {
    //     debugs(33, DBG_CRITICAL, "Bad conn" << PQerrorMessage(conn));
    // }
    // PQfinish(conn);
    // return 0;
}
void 
QuotaDB::UpdateQuota(const char *username, int quota)
{
    sprintf(query, "UPDATE %s SET cuota_internet=%d WHERE correo='%s'", TABLE, quota, username);
    res = PQexec(conn,query);
    PQclear(res);
}
void
QuotaDB::ResetConsumed(const char *username) {
    sprintf(query, "UPDATE %s SET consumido=%d WHERE correo='%s'", TABLE, 0, username);
    res = PQexec(conn,query);
    PQclear(res);
    // sprintf(query, "dbname=%s host=%s user=%s password=%s", DBNAME, HOST, USER, PASS);
    // conn = PQconnectdb(query);

    // if(PQstatus(conn) == CONNECTION_OK)
    // {
    // sprintf(query, "SELECT consumido FROM %s WHERE correo='%s'", TABLE, username);
    // int consumed = 0;
    // res = PQexec(conn,query);
    // if (PQresultStatus(res) == PGRES_TUPLES_OK) {
    //     consumed= atoll(PQgetvalue(res, 0, 0));
    // }
    // else {
    //     printf("Error: %s",PQresultErrorMessage(res));
    // }
    // PQclear(res);
    // // PQfinish(conn);
    // return consumed;
    // } else {
    //     debugs(33, DBG_CRITICAL, "" << PQerrorMessage(conn));
    // }
    // PQfinish(conn);
    // return 0;
}
bool 
QuotaDB::Overquota(const char *username)
{
    sprintf(query, "SELECT cuota_internet, consumido FROM %s WHERE correo='%s'", TABLE, username);
    int quota = 0;
    long long int consumed = 0;
    res = PQexec(conn,query);
    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
    debugs(33, DBG_IMPORTANT, "query is ok");
        quota = atoi(PQgetvalue(res, 0, 0));
        consumed = atoll(PQgetvalue(res, 0, 1));
        debugs(33, DBG_IMPORTANT, "this is quota " << (int)quota);
    }
    else {
    debugs(33, DBG_CRITICAL, "bad query" << PQresultErrorMessage(res));
        printf("Error: %s",PQresultErrorMessage(res));
    }
    PQclear(res);
    return (consumed/1048576) > quota;
}