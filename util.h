#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <sstream>
using namespace std;
enum Role {ROLE_NONE=0, ROLE_BUYER=1, ROLE_SELLER=2};
enum TxStatus {STATUS_PAID=0, STATUS_COMPLETED=1, STATUS_CANCELLED=2};
string now_iso();
time_t parse_time(const string& s);
int days_between(time_t a, time_t b);
string status_to_string(TxStatus s);
#endif
