#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <time.h>
#include "globals.h"
#include "SquidTime.h"


#define HOST "localhost"
#define USER "squid2"
#define PASS "squid"
#define DB "quotas_db"
#define DBNAME "quotas_quota"


class QuotaDB
{
public:
    QuotaDB();
    ~QuotaDB();
    float saveSize(char *user, float mb_size, time_t curr_time);
    int quota(char *user);
    void findUser(const char *user, UserInfo& userInfo);

private:
    MYSQL *conn;
    MYSQL_ROW row;
    MYSQL_RES *res;
    char query[1024];
};

QuotaDB::QuotaDB()
{
    conn = mysql_init(NULL);
    /* connect to server */
    if (!mysql_real_connect(conn, HOST, USER, PASS, DB, 3306, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        mysql_close (conn);
    }
}

QuotaDB::~QuotaDB()
{
    mysql_close(conn);
}