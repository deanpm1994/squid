#ifndef __USERINFO_H__
#define __USERINFO_H__

#include "cbdata.h"
#include "enums.h"
#include "hash.h"
#include "typedefs.h"
#include <deque>
#include "SquidTime.h"


class UserInfo {
public:
    hash_link hash;
    const char *username;
    // const char *mod_time;
    int quota;
    long long int current;
    long long int tunnel;
    time_t expiretime;
    UserInfo(const char *user, int q, long long int c, long long int t, time_t exp)
    {
        this->hash.key = xstrdup(user);
        this->username = user;
        this->quota = q;
        this->current = c;
        this->tunnel = t;
        this->expiretime = exp;
    }
    ~UserInfo() 
    {
        xfree((char*)username);
        xfree(hash.key);
        xfree(this);
    }
};

#endif