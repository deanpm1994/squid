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
    int quota;
    // float current;
    float monthly;
    int lmonth;
    int lyear;
    time_t expiretime;
    UserInfo();
    ~UserInfo();
};

#endif