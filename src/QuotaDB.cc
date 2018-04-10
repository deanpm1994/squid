#include "squid.h"
#include <stdio.h>
#include <stdlib.h>
#include "mysql.h"
#include <time.h>
// #include "globals.h"
// #include "SquidTime.h"
#include "UserInfo.h"

#include "QuotaDB.h"

void
QuotaDB::findUser(const char *user, UserInfo* userInfo) {
    sprintf(query, "SELECT quota, monthly, last_month, last_year FROM %s WHERE user='%s'", DBNAME, user);
    if (mysql_query(conn, query)==0) {
        res = mysql_use_result(conn);
        while ((row=mysql_fetch_row(res)))
        {
            userInfo->hash.key = (void *)user;
            userInfo->username = user;
            userInfo->quota = atoi(row[0]);
            // userInfo.current = 0;
            userInfo->monthly = atof(row[1]);
            userInfo->lmonth = atoi(row[2]);
            userInfo->lyear = atoi(row[3]);
            userInfo->expiretime = current_time.tv_sec;
        }
        if (!mysql_eof(res)){
            printf("Lecture error: [%s]\n", mysql_error(conn));
        }
    }
} 
int
QuotaDB::quota(const char *user) {
    sprintf(query, "SELECT quota FROM %s WHERE user='%s'", DBNAME, user);
    int quota = 0;
    if (mysql_query(conn, query)==0) {
        res = mysql_use_result(conn);
        while ((row=mysql_fetch_row(res)))
        {
            quota = atof(row[0]);
        }
        if (!mysql_eof(res)){
            printf("Lecture error: [%s]\n", mysql_error(conn));
        }
    }
    return quota;
}

float
QuotaDB::saveSize(const char *user, float mb_size, time_t curr_time) {
    sprintf(query, "SELECT daily, weekly, monthly, last_mday, last_month, last_year, last_wday FROM %s WHERE user='%s'", DBNAME, user);
    float daily = 0; float weekly = 0; float monthly = 0;
    float to_daily = 0; float to_weekly = 0; float to_monthly = 0;
    int last_mday = 0; int last_month = 0; int last_year = 0; int last_wday = 0;
    if (mysql_query(conn, query)==0) {
        res = mysql_use_result(conn);
        while ((row=mysql_fetch_row(res)))
        {
            daily = atof(row[0]); 
            weekly = atof(row[1]); 
            monthly = atof(row[2]);
            last_mday = atoi(row[3]); 
            last_month = atoi(row[4]); 
            last_year = atoi(row[5]);
            last_wday = atoi(row[6]);
        }
        if (!mysql_eof(res)){
            printf("Lecture error: [%s]\n", mysql_error(conn));
        }
    }
    time(&curr_time);
    struct tm now = *localtime(&curr_time);
    if (now.tm_year + 1900 == last_year) {
        if (now.tm_mon == last_month) {
            if (now.tm_mday == last_mday) {
                to_daily = daily + mb_size;
                to_weekly = weekly + mb_size;
                to_monthly = monthly + mb_size;
            }
            else if (now.tm_mday - last_mday < 7 && now.tm_wday - last_wday > 0)
            {
                to_daily = mb_size;
                to_weekly = weekly + mb_size;
                to_monthly = monthly + mb_size;
            }
            else {
                to_daily = mb_size;
                to_weekly = mb_size;
                to_monthly = monthly + mb_size;
            }
        }
        else {
            to_daily = mb_size;
            to_weekly = mb_size;
            to_monthly = mb_size;
        }
    }
    else {
        to_daily = mb_size;
        to_weekly = mb_size;
        to_monthly = mb_size;
    }
    sprintf(query, "UPDATE %s SET daily='%.2f', weekly='%.2f', monthly='%.2f', last_mday='%d', last_month='%d', last_year='%d', last_wday='%d' WHERE user='%s'",
        DBNAME, to_daily, to_weekly, to_monthly,
        now.tm_mday, now.tm_mon, now.tm_year + 1900, now.tm_wday, user);
    mysql_query(conn, query);
    return to_monthly;
}  


