#include "datastore.h"
#include <fstream>
#include <sstream>
#include <string>
DataStore storeData;
void DataStore::save_all()
{
    ofstream ou("users.db");
    for(auto &u: users) ou<<u.serialize()<<"\n";
    ou.close();
    ofstream ob("banks.db");
    for(auto &b: banks) ob<<b.serialize()<<"\n";
    ob.close();
    ofstream oi("items.db");
    for(auto &it: items) oi<<it.serialize()<<"\n";
    oi.close();
    ofstream ot("txs.db");
    for(auto &t: txs) ot<<t.serialize()<<"\n";
    ot.close();
    ofstream meta("meta.db");
    meta<<nextUserId<<","<<nextBankId<<","<<nextItemId<<","<<nextTxId<<"\n";
    meta.close();
}
void DataStore::load_all()
{
    users.clear(); banks.clear(); items.clear(); txs.clear();
    ifstream iu("users.db");
    string line;
    while(getline(iu,line)){ if(line.size()) users.push_back(User::deserialize(line)); }
    iu.close();
    ifstream ib("banks.db");
    while(getline(ib,line)){ if(line.size()) banks.push_back(BankAccount::deserialize(line)); }
    ib.close();
    ifstream ii("items.db");
    while(getline(ii,line)){ if(line.size()) items.push_back(Item::deserialize(line)); }
    ii.close();
    ifstream it("txs.db");
    while(getline(it,line)){ if(line.size()) txs.push_back(Transaction::deserialize(line)); }
    it.close();
    ifstream im("meta.db");
    if(getline(im,line))
    {
        stringstream ss(line);
        string tok;
        getline(ss,tok,','); nextUserId = stoi(tok);
        getline(ss,tok,','); nextBankId = stoi(tok);
        getline(ss,tok,','); nextItemId = stoi(tok);
        getline(ss,tok); nextTxId = stoi(tok);
    }
    im.close();
    if(nextUserId<=0) nextUserId=1;
    if(nextBankId<=0) nextBankId=1;
    if(nextItemId<=0) nextItemId=1;
    if(nextTxId<=0) nextTxId=1;
}
User* DataStore::find_user_by_name(const string& name)
{
    for(auto &u: users) if(u.username==name) return &u;
    return nullptr;
}
User* DataStore::find_user_by_id(int id)
{
    for(auto &u: users) if(u.id==id) return &u;
    return nullptr;
}
BankAccount* DataStore::find_bank_by_id(int id)
{
    for(auto &b: banks) if(b.id==id) return &b;
    return nullptr;
}
BankAccount* DataStore::find_bank_by_owner(int uid)
{
    for(auto &b: banks) if(b.ownerUserId==uid) return &b;
    return nullptr;
}
Item* DataStore::find_item_by_id(int id)
{
    for(auto &it: items) if(it.id==id) return &it;
    return nullptr;
}
vector<Transaction*> DataStore::txs_by_user(int uid)
{
    vector<Transaction*> out;
    for(auto &t: txs) if(t.buyerId==uid || t.sellerId==uid) out.push_back(&t);
    return out;
}
vector<Transaction*> DataStore::txs_by_date_range(time_t from, time_t to)
{
    vector<Transaction*> out;
    for(auto &t: txs)
    {
        time_t tt = parse_time(t.timeIso);
        if(tt >= from && tt <= to) out.push_back(&t);
    }
    return out;
}
