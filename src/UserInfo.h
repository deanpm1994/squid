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
    const char *mod_time;
    int quota;
    float current;
    time_t expiretime;
};

#endif