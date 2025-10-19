#include "util.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cstring>
string now_iso()
{
    time_t t = time(nullptr);
    struct tm tm;
    localtime_r(&t, &tm);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return string(buf);
}
time_t parse_time(const string& s)
{
    struct tm tm{};
    strptime(s.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    return mktime(&tm);
}
int days_between(time_t a, time_t b)
{
    double diff = difftime(a,b);
    return (int)floor(abs(diff) / (60*60*24));
}
string status_to_string(TxStatus s)
{
    if(s==STATUS_PAID) return "PAID";
    if(s==STATUS_COMPLETED) return "COMPLETED";
    return "CANCELLED";
}
