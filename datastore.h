#ifndef DATASTORE_H
#define DATASTORE_H
#include <vector>
#include "entities.h"
using namespace std;
struct DataStore
{
    vector<User> users;
    vector<BankAccount> banks;
    vector<Item> items;
    vector<Transaction> txs;
    int nextUserId;
    int nextBankId;
    int nextItemId;
    int nextTxId;
    DataStore():nextUserId(1),nextBankId(1),nextItemId(1),nextTxId(1){}
    void save_all();
    void load_all();
    User* find_user_by_name(const string& name);
    User* find_user_by_id(int id);
    BankAccount* find_bank_by_id(int id);
    BankAccount* find_bank_by_owner(int uid);
    Item* find_item_by_id(int id);
    vector<Transaction*> txs_by_user(int uid);
    vector<Transaction*> txs_by_date_range(time_t from, time_t to);
};
extern DataStore storeData;
#endif
