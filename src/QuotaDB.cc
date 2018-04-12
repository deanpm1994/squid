#include "squid.h"
#include <stdio.h>
#include <stdlib.h>
#include "postgresql/libpq-fe.h"
#include <time.h>
// #include "globals.h"
// #include "SquidTime.h"
// #include "UserInfo.h"

#include "QuotaDB.h"

void QuotaDB::SaveData(const char *username, float current, const char *mod_time) {
    sprintf(query, "update %s set consumed=%.2f modificated=%s where user=%s", DBNAME, current, mod_time, username);
    res = PQexec(conn,query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Error: %s",PQresultErrorMessage(res));
    }
    PQclear(res);
}

void
QuotaDB::Find(const char *username, UserInfo *userInfo) {
    sprintf(query, "select quota,consumed,modificated from %s where user='%s'", DBNAME, username);
    res = PQexec(conn,query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        userInfo->hash.key = (void *)username;
        userInfo->username = username;
        userInfo->tunnel = 19;
        userInfo->quota = atoi(PQgetvalue(res, 0, 0));
        userInfo->current = atof(PQgetvalue(res, 0, 1));
        userInfo->mod_time = PQgetvalue(res, 0, 2);
    }
    else {
        printf("Error: %s",PQresultErrorMessage(res));
    }
    PQclear(res);
} 
int
QuotaDB::Quota(const char *username) {
    sprintf(query, "select quota from %s where user=%s", DBNAME, username);
    int quota = 0;
    res = PQexec(conn,query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        quota = atoi(PQgetvalue(res, 0, 0));
    }
    else {
        printf("Error: %s",PQresultErrorMessage(res));
    }
    PQclear(res);
    return quota;
}
float
QuotaDB::Consumed(const char *username) {
    sprintf(query, "select consumed from %s where user=%s", DBNAME, username);
    int consumed = 0;
    res = PQexec(conn,query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        consumed= atof(PQgetvalue(res, 0, 0));
    }
    else {
        printf("Error: %s",PQresultErrorMessage(res));
    }
    PQclear(res);
    return consumed;
}
// float
// QuotaDB::saveSize(const char *user, float mb_size, time_t curr_time) {
//     sprintf(query, "SELECT daily, weekly, monthly, last_mday, last_month, last_year, last_wday FROM %s WHERE user='%s'", DBNAME, user);
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


