#include "menu.h"
#include "util.h"
#include <iostream>
#include <limits>
#include <map>
#include <algorithm>
#include <iomanip>
using namespace std;
int input_int()
{
    int x;
    while(!(cin>>x)){ cin.clear(); cin.ignore(numeric_limits<streamsize>::max(),'\n'); cout<<"Invalid input. Try again: "; }
    cin.ignore(numeric_limits<streamsize>::max(),'\n');
    return x;
}
double input_double()
{
    double x;
    while(!(cin>>x)){ cin.clear(); cin.ignore(numeric_limits<streamsize>::max(),'\n'); cout<<"Invalid input. Try again: "; }
    cin.ignore(numeric_limits<streamsize>::max(),'\n');
    return x;
}
void register_user()
{
    cout<<"Register as Buyer(1) or Seller(2): ";
    int r = input_int();
    if(r!=1 && r!=2){ cout<<"Invalid role\n"; return; }
    cout<<"Username: "; string uname; getline(cin,uname);
    if(uname.empty()){ getline(cin,uname); }
    if(storeData.find_user_by_name(uname)){ cout<<"Username exists\n"; return; }
    cout<<"Password: "; string pwd; getline(cin,pwd);
    User u;
    u.id = storeData.nextUserId++;
    u.username = uname;
    u.password = pwd;
    u.role = (r==1?ROLE_BUYER:ROLE_SELLER);
    u.bankAccountId = 0;
    storeData.users.push_back(u);
    storeData.save_all();
    cout<<"Registered user id "<<u.id<<"\n";
}
User* login_user()
{
    cout<<"Username: "; string uname; getline(cin,uname);
    if(uname.empty()){ getline(cin,uname); }
    cout<<"Password: "; string pwd; getline(cin,pwd);
    User* u = storeData.find_user_by_name(uname);
    if(!u){ cout<<"No such user\n"; return nullptr; }
    if(u->password!=pwd){ cout<<"Wrong password\n"; return nullptr; }
    cout<<"Welcome "<<u->username<<"\n";
    return u;
}
void create_bank_account(User* u)
{
    if(!u) return;
    if(u->bankAccountId!=0){ cout<<"Already have bank account id "<<u->bankAccountId<<"\n"; return; }
    BankAccount b;
    b.id = storeData.nextBankId++;
    b.ownerUserId = u->id;
    b.balance = 0;
    b.lastActive = now_iso();
    storeData.banks.push_back(b);
    u->bankAccountId = b.id;
    storeData.save_all();
    cout<<"Bank account created id "<<b.id<<"\n";
}
void topup(User* u)
{
    if(!u) return;
    BankAccount* b = storeData.find_bank_by_owner(u->id);
    if(!b){ cout<<"No bank account. Create one first.\n"; return; }
    cout<<"Amount to topup: "; double amt = input_double();
    if(amt<=0){ cout<<"Invalid amount\n"; return; }
    b->balance += amt;
    b->lastActive = now_iso();
    storeData.save_all();
    cout<<"Topup successful. Balance: "<<fixed<<setprecision(2)<<b->balance<<"\n";
}
void withdraw(User* u)
{
    if(!u) return;
    BankAccount* b = storeData.find_bank_by_owner(u->id);
    if(!b){ cout<<"No bank account. Create one first.\n"; return; }
    cout<<"Amount to withdraw: "; double amt = input_double();
    if(amt<=0 || amt > b->balance){ cout<<"Invalid amount or insufficient balance\n"; return; }
    b->balance -= amt;
    b->lastActive = now_iso();
    storeData.save_all();
    cout<<"Withdraw successful. Balance: "<<fixed<<setprecision(2)<<b->balance<<"\n";
}
void list_orders(User* u)
{
    if(!u) return;
    for(auto &t: storeData.txs)
    {
        if(t.buyerId==u->id)
        {
            cout<<"Tx "<<t.id<<" Item "<<t.itemId<<" Qty "<<t.qty<<" Amount "<<t.amount<<" Time "<<t.timeIso<<" Status "<<status_to_string(t.status)<<"\n";
        }
    }
}
void filter_orders(User* u)
{
    if(!u) return;
    cout<<"Filter by status: 0=PAID 1=COMPLETED 2=CANCELLED: ";
    int s = input_int();
    if(s<0||s>2){ cout<<"Invalid\n"; return; }
    for(auto &t: storeData.txs)
    {
        if(t.buyerId==u->id && int(t.status)==s)
        {
            cout<<"Tx "<<t.id<<" Item "<<t.itemId<<" Qty "<<t.qty<<" Amount "<<t.amount<<" Time "<<t.timeIso<<"\n";
        }
    }
}
void spending_last_k_days(User* u)
{
    if(!u) return;
    cout<<"k days: "; int k = input_int();
    time_t now = time(nullptr);
    double total=0;
    for(auto &t: storeData.txs)
    {
        if(t.buyerId==u->id)
        {
            time_t tt = parse_time(t.timeIso);
            if(difftime(now,tt) <= k*24*60*60) total += t.amount;
        }
    }
    cout<<"Spending last "<<k<<" days: "<<fixed<<setprecision(2)<<total<<"\n";
}
void buyer_menu(User* u)
{
    while(true)
    {
        cout<<"\nBuyer Menu: 1-CreateBank 2-Topup 3-Withdraw 4-Purchase 5-ListOrders 6-FilterOrders 7-SpendingK 8-Logout\nChoice: ";
        int c = input_int();
        if(c==1) create_bank_account(u);
        else if(c==2) topup(u);
        else if(c==3) withdraw(u);
        else if(c==4)
        {
            cout<<"Available items:\n";
            for(auto &it: storeData.items)
            {
                cout<<"ItemId "<<it.id<<" Name "<<it.name<<" Price "<<it.price<<" Stock "<<it.stock<<" Seller "<<it.sellerId<<"\n";
            }
            cout<<"Item id to buy: "; int iid = input_int();
            Item* it = storeData.find_item_by_id(iid);
            if(!it){ cout<<"No item\n"; continue; }
            cout<<"Qty: "; int q = input_int();
            if(q<=0 || q>it->stock){ cout<<"Invalid qty\n"; continue; }
            BankAccount* b = storeData.find_bank_by_owner(u->id);
            if(!b){ cout<<"No bank account\n"; continue; }
            double total = q * it->price;
            if(b->balance < total){ cout<<"Insufficient balance\n"; continue; }
            b->balance -= total;
            b->lastActive = now_iso();
            it->stock -= q;
            it->totalSold += q;
            Transaction tx;
            tx.id = storeData.nextTxId++;
            tx.buyerId = u->id;
            tx.sellerId = it->sellerId;
            tx.itemId = it->id;
            tx.qty = q;
            tx.amount = total;
            tx.timeIso = now_iso();
            tx.status = STATUS_PAID;
            storeData.txs.push_back(tx);
            storeData.save_all();
            cout<<"Purchase recorded Tx id "<<tx.id<<"\n";
        }
        else if(c==5) list_orders(u);
        else if(c==6) filter_orders(u);
        else if(c==7) spending_last_k_days(u);
        else if(c==8) break;
        else cout<<"Invalid\n";
    }
}
void seller_add_item(User* u)
{
    if(!u) return;
    cout<<"Item name: "; string name; getline(cin,name);
    if(name.empty()){ getline(cin,name); }
    cout<<"Price: "; double p = input_double();
    cout<<"Stock: "; int s = input_int();
    Item it;
    it.id = storeData.nextItemId++;
    it.sellerId = u->id;
    it.name = name;
    it.price = p;
    it.stock = s;
    it.totalSold = 0;
    storeData.items.push_back(it);
    storeData.save_all();
    cout<<"Item added id "<<it.id<<"\n";
}
void seller_manage_item(User* u)
{
    if(!u) return;
    cout<<"Your items:\n";
    for(auto &it: storeData.items) if(it.sellerId==u->id) cout<<"Id "<<it.id<<" "<<it.name<<" Stock "<<it.stock<<" Price "<<it.price<<"\n";
    cout<<"Item id to manage: "; int iid = input_int();
    Item* it = storeData.find_item_by_id(iid);
    if(!it || it->sellerId!=u->id){ cout<<"No such item\n"; return; }
    cout<<"1-Replenish 2-Discard 3-SetPrice: ";
    int c = input_int();
    if(c==1){ cout<<"Amount: "; int a = input_int(); if(a>0) it->stock += a; }
    else if(c==2){ it->stock = 0; }
    else if(c==3){ cout<<"New price: "; double p = input_double(); if(p>0) it->price = p; }
    storeData.save_all();
    cout<<"Updated\n";
}
void seller_discover_popular(User* u)
{
    if(!u) return;
    cout<<"Top k popular items per month. k: "; int k = input_int();
    map<int,int> counts;
    time_t now = time(nullptr);
    for(auto &t: storeData.txs)
    {
        time_t tt = parse_time(t.timeIso);
        double diff = difftime(now, tt);
        if(diff <= 30*24*60*60)
        {
            if(t.sellerId==u->id) counts[t.itemId] += t.qty;
        }
    }
    vector<pair<int,int>> vec;
    for(auto &p: counts) vec.push_back(p);
    sort(vec.begin(), vec.end(), [](auto &a, auto &b){ return a.second>b.second; });
    for(int i=0;i<k && i<(int)vec.size();++i)
    {
        Item* it = storeData.find_item_by_id(vec[i].first);
        if(it) cout<<"Item "<<it->id<<" "<<it->name<<" sold "<<vec[i].second<<"\n";
    }
}
void seller_loyal_customers(User* u)
{
    if(!u) return;
    cout<<"Top k loyal customers per month. k: "; int k = input_int();
    map<int,int> counts;
    time_t now = time(nullptr);
    for(auto &t: storeData.txs)
    {
        time_t tt = parse_time(t.timeIso);
        double diff = difftime(now, tt);
        if(diff <= 30*24*60*60)
        {
            if(t.sellerId==u->id) counts[t.buyerId] += 1;
        }
    }
    vector<pair<int,int>> vec;
    for(auto &p: counts) vec.push_back(p);
    sort(vec.begin(), vec.end(), [](auto &a, auto &b){ return a.second>b.second; });
    for(int i=0;i<k && i<(int)vec.size();++i)
    {
        User* u2 = storeData.find_user_by_id(vec[i].first);
        cout<<"Buyer "<<(u2?u2->username:to_string(vec[i].first))<<" txs "<<vec[i].second<<"\n";
    }
}
void seller_menu(User* u)
{
    while(true)
    {
        cout<<"\nSeller Menu: 1-CreateBank 2-Topup 3-Withdraw 4-AddItem 5-ManageItem 6-DiscoverPopular 7-LoyalCustomers 8-ListOrders 9-Logout\nChoice: ";
        int c = input_int();
        if(c==1) create_bank_account(u);
        else if(c==2) topup(u);
        else if(c==3) withdraw(u);
        else if(c==4) seller_add_item(u);
        else if(c==5) seller_manage_item(u);
        else if(c==6) seller_discover_popular(u);
        else if(c==7) seller_loyal_customers(u);
        else if(c==8) list_orders(u);
        else if(c==9) break;
        else cout<<"Invalid\n";
    }
}
void store_list_latest_k_days()
{
    cout<<"k days: "; int k = input_int();
    time_t now = time(nullptr);
    for(auto &t: storeData.txs)
    {
        time_t tt = parse_time(t.timeIso);
        if(difftime(now, tt) <= k*24*60*60)
        {
            cout<<"Tx "<<t.id<<" Buyer "<<t.buyerId<<" Seller "<<t.sellerId<<" Item "<<t.itemId<<" Amt "<<t.amount<<" Time "<<t.timeIso<<" Status "<<status_to_string(t.status)<<"\n";
        }
    }
}
void store_list_paid_not_completed()
{
    for(auto &t: storeData.txs)
    {
        if(t.status==STATUS_PAID) cout<<"Tx "<<t.id<<" Buyer "<<t.buyerId<<" Seller "<<t.sellerId<<" Item "<<t.itemId<<" Amt "<<t.amount<<" Time "<<t.timeIso<<"\n";
    }
}
void store_most_m_frequent_items()
{
    cout<<"m: "; int m = input_int();
    map<int,int> counts;
    for(auto &t: storeData.txs) counts[t.itemId] += t.qty;
    vector<pair<int,int>> vec;
    for(auto &p: counts) vec.push_back(p);
    sort(vec.begin(), vec.end(), [](auto &a, auto &b){ return a.second>b.second; });
    for(int i=0;i<m && i<(int)vec.size();++i)
    {
        Item* it = storeData.find_item_by_id(vec[i].first);
        cout<<"Item "<<(it?it->name:to_string(vec[i].first))<<" sold "<<vec[i].second<<"\n";
    }
}
void store_most_active_buyers_per_day()
{
    map<string,map<int,int>> dayBuyerCount;
    for(auto &t: storeData.txs)
    {
        string day = t.timeIso.substr(0,10);
        dayBuyerCount[day][t.buyerId] += 1;
    }
    map<string, pair<int,int>> best;
    for(auto &p: dayBuyerCount)
    {
        int bestCnt=0, bestId=0;
        for(auto &q: p.second) if(q.second>bestCnt){ bestCnt=q.second; bestId=q.first; }
        best[p.first] = make_pair(bestId,bestCnt);
    }
    for(auto &p: best) cout<<"Day "<<p.first<<" BuyerId "<<p.second.first<<" txs "<<p.second.second<<"\n";
}
void store_most_active_sellers_per_day()
{
    map<string,map<int,int>> daySellerCount;
    for(auto &t: storeData.txs)
    {
        string day = t.timeIso.substr(0,10);
        daySellerCount[day][t.sellerId] += 1;
    }
    for(auto &p: daySellerCount)
    {
        int bestCnt=0,bestId=0;
        for(auto &q: p.second) if(q.second>bestCnt){ bestCnt=q.second; bestId=q.first; }
        cout<<"Day "<<p.first<<" SellerId "<<bestId<<" txs "<<bestCnt<<"\n";
    }
}
void bank_list_week_backwards()
{
    time_t now = time(nullptr);
    time_t weekAgo = now - 7*24*60*60;
    for(auto &t: storeData.txs)
    {
        time_t tt = parse_time(t.timeIso);
        if(tt >= weekAgo && tt <= now) cout<<"Tx "<<t.id<<" Time "<<t.timeIso<<" Buyer "<<t.buyerId<<" Seller "<<t.sellerId<<" Amt "<<t.amount<<"\n";
    }
}
void bank_list_all_customers()
{
    for(auto &b: storeData.banks)
    {
        User* u = storeData.find_user_by_id(b.ownerUserId);
        cout<<"BankId "<<b.id<<" Owner "<<(u?u->username:to_string(b.ownerUserId))<<" Balance "<<b.balance<<" LastActive "<<b.lastActive<<"\n";
    }
}
void bank_list_dormant_accounts()
{
    time_t now = time(nullptr);
    for(auto &b: storeData.banks)
    {
        time_t lt = parse_time(b.lastActive);
        if(difftime(now, lt) >= 30*24*60*60) cout<<"Dormant BankId "<<b.id<<" Owner "<<b.ownerUserId<<"\n";
    }
}
void bank_top_n_users_today()
{
    cout<<"n: "; int n = input_int();
    map<int,int> cnt;
    time_t now = time(nullptr);
    string today = now_iso().substr(0,10);
    for(auto &t: storeData.txs)
    {
        if(t.timeIso.substr(0,10)==today) cnt[t.buyerId] += 1;
    }
    vector<pair<int,int>> vec;
    for(auto &p: cnt) vec.push_back(p);
    sort(vec.begin(), vec.end(), [](auto &a, auto &b){ return a.second>b.second; });
    for(int i=0;i<n && i<(int)vec.size();++i) cout<<"Buyer "<<vec[i].first<<" txs "<<vec[i].second<<"\n";
}
void admin_store_menu()
{
    while(true)
    {
        cout<<"\nStore Admin: 1-ListLatestKDays 2-ListPaidNotCompleted 3-MostFrequentItems 4-MostActiveBuyersPerDay 5-MostActiveSellersPerDay 6-Back\nChoice: ";
        int c = input_int();
        if(c==1) store_list_latest_k_days();
        else if(c==2) store_list_paid_not_completed();
        else if(c==3) store_most_m_frequent_items();
        else if(c==4) store_most_active_buyers_per_day();
        else if(c==5) store_most_active_sellers_per_day();
        else if(c==6) break;
        else cout<<"Invalid\n";
    }
}
void admin_bank_menu()
{
    while(true)
    {
        cout<<"\nBank Admin: 1-ListWeekBackwards 2-ListAllCustomers 3-ListDormantAccounts 4-TopNUsersToday 5-Back\nChoice: ";
        int c = input_int();
        if(c==1) bank_list_week_backwards();
        else if(c==2) bank_list_all_customers();
        else if(c==3) bank_list_dormant_accounts();
        else if(c==4) bank_top_n_users_today();
        else if(c==5) break;
        else cout<<"Invalid\n";
    }
}
void mark_transaction_completed()
{
    cout<<"Tx id to mark completed: "; int id = input_int();
    for(auto &t: storeData.txs)
    {
        if(t.id==id)
        {
            t.status = STATUS_COMPLETED;
            storeData.save_all();
            cout<<"Updated\n";
            return;
        }
    }
    cout<<"Not found\n";
}
void cancel_transaction()
{
    cout<<"Tx id to cancel: "; int id = input_int();
    for(auto &t: storeData.txs)
    {
        if(t.id==id)
        {
            t.status = STATUS_CANCELLED;
            BankAccount* b = storeData.find_bank_by_owner(t.buyerId);
            if(b){ b->balance += t.amount; b->lastActive = now_iso(); }
            Item* it = storeData.find_item_by_id(t.itemId);
            if(it){ it->stock += t.qty; it->totalSold -= t.qty; }
            storeData.save_all();
            cout<<"Cancelled and refunded if possible\n";
            return;
        }
    }
    cout<<"Not found\n";
}
void load_seed_if_empty()
{
    if(storeData.users.empty())
    {
        User admin;
        admin.id = storeData.nextUserId++;
        admin.username = "admin";
        admin.password = "admin";
        admin.role = ROLE_NONE;
        admin.bankAccountId = 0;
        storeData.users.push_back(admin);
        storeData.save_all();
    }
}
