#include "squid.h"
#include <stdio.h>
#include <stdlib.h>
#include "postgresql/libpq-fe.h"
#include <time.h>
// #include "globals.h"
// #include "SquidTime.h"
// #include "UserInfo.h"

#include "QuotaDB.h"

void QuotaDB::SaveData(const char *username, int current) {
    sprintf(query, "dbname=%s host=%s user=%s password=%s", DBNAME, HOST, USER, PASS);
    conn = PQconnectdb(query);

    if(PQstatus(conn) == CONNECTION_OK)
    {
        sprintf(query, "UPDATE %s SET consumido=%d WHERE correo='%s'", TABLE, current, username);
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
            // // try
            // // {
            // //     userInfo->hash.key = xstrdup(username);
            // // }
            // // catch (const std::exception& e)
            // // {
            // //     debugs(33, DBG_CRITICAL, "" << e.what());
            // // }
            // // debugs(33, DBG_IMPORTANT, "user hash.key " << userInfo->hash.key);
            // debugs(33, DBG_IMPORTANT, "Inside If to create user");
            // userInfo->username = username;
            // debugs(33, DBG_IMPORTANT, "Inside If to create user");
            // userInfo->tunnel = 19;
            // debugs(33, DBG_IMPORTANT, "Before PQgetvalue");
            // userInfo->quota = atoi(PQgetvalue(res, 0, 0));
            // userInfo->current = atoi(PQgetvalue(res, 0, 1));
            debugs(33, DBG_IMPORTANT, "Find----" << PQgetvalue(res,0,1));
            user = new UserInfo(username, atoi(PQgetvalue(res, 0, 0)), atoi(PQgetvalue(res, 0, 1)), 19, squid_curtime);
            debugs(33, DBG_IMPORTANT, "After");
            // userInfo->mod_time = PQgetvalue(res, 0, 2);
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
int
QuotaDB::Consumed(const char *username) {
    sprintf(query, "dbname=%s host=%s user=%s password=%s", DBNAME, HOST, USER, PASS);
    conn = PQconnectdb(query);

    if(PQstatus(conn) == CONNECTION_OK)
    {
        sprintf(query, "SELECT consumido FROM %s WHERE correo='%s'", TABLE, username);
        int consumed = 0;
        res = PQexec(conn,query);
        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            consumed= atoi(PQgetvalue(res, 0, 0));
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
// float
// QuotaDB::saveSize(const char *user, float mb_size, time_t curr_time) {
//     sprintf(query, "SELECT daily, weekly, monthly, last_mday, last_month, last_year, last_wday FROM %s WHERE user='%s'", TABLE, user);
//     float daily = 0; float weekly = 0; float monthly = 0;
//     float to_daily = 0; float to_weekly = 0; float to_monthly = 0;
//     int last_mday = 0; int last_month = 0; int last_year = 0; int last_wday = 0;
//     if (mysql_query(conn, query)==0) {
//         res = mysql_use_result(conn);
//         while ((row=mysql_fetch_row(res)))
//         {
//             daily = atof(row[0]); 
//             weekly = atof(row[1]); 
//             monthly = atof(row[2]);
//             last_mday = atoi(row[3]); 
//             last_month = atoi(row[4]); 
//             last_year = atoi(row[5]);
//             last_wday = atoi(row[6]);
//         }
//         if (!mysql_eof(res)){
//             printf("Lecture error: [%s]\n", mysql_error(conn));
//         }
//     }
//     time(&curr_time);
//     struct tm now = *localtime(&curr_time);
//     if (now.tm_year + 1900 == last_year) {
//         if (now.tm_mon == last_month) {
//             if (now.tm_mday == last_mday) {
//                 to_daily = daily + mb_size;
//                 to_weekly = weekly + mb_size;
//                 to_monthly = monthly + mb_size;
//             }
//             else if (now.tm_mday - last_mday < 7 && now.tm_wday - last_wday > 0)
//             {
//                 to_daily = mb_size;
//                 to_weekly = weekly + mb_size;
//                 to_monthly = monthly + mb_size;
//             }
//             else {
//                 to_daily = mb_size;
//                 to_weekly = mb_size;
//                 to_monthly = monthly + mb_size;
//             }
//         }
//         else {
//             to_daily = mb_size;
//             to_weekly = mb_size;
//             to_monthly = mb_size;
//         }
//     }
//     else {
//         to_daily = mb_size;
//         to_weekly = mb_size;
//         to_monthly = mb_size;
//     }
//     sprintf(query, "UPDATE %s SET daily='%.2f', weekly='%.2f', monthly='%.2f', last_mday='%d', last_month='%d', last_year='%d', last_wday='%d' WHERE user='%s'",
//         DBNAME, to_daily, to_weekly, to_monthly,
//         now.tm_mday, now.tm_mon, now.tm_year + 1900, now.tm_wday, user);
//     mysql_query(conn, query);
//     return to_monthly;
// }  


