#include "hash.h"
#include "SquidTime.h"

class UserInfo {
public:
    hash_link hash;
    char *username;
    int quota;
    // float current;
    float monthly;
    int lmonth;
    int lyear;
    time_t expiretime;
    UserInfo();
    ~UserInfo();
}