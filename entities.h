#ifndef ENTITIES_H
#define ENTITIES_H
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;
#include "util.h"
struct BankAccount
{
    int id;
    int ownerUserId;
    double balance;
    string lastActive;
    BankAccount():id(0),ownerUserId(0),balance(0),lastActive(now_iso()){}
    string serialize() const
    {
        ostringstream oss;
        oss<<id<<","<<ownerUserId<<","<<fixed<<setprecision(2)<<balance<<","<<lastActive;
        return oss.str();
    }
    static BankAccount deserialize(const string& line)
    {
        BankAccount b;
        stringstream ss(line);
        string tok;
        getline(ss,tok,','); b.id = stoi(tok);
        getline(ss,tok,','); b.ownerUserId = stoi(tok);
        getline(ss,tok,','); b.balance = stod(tok);
        getline(ss,tok); b.lastActive = tok;
        return b;
    }
};
struct User
{
    int id;
    string username;
    string password;
    Role role;
    int bankAccountId;
    User():id(0),role(ROLE_NONE),bankAccountId(0){}
    string serialize() const
    {
        ostringstream oss;
        oss<<id<<","<<username<<","<<password<<","<<int(role)<<","<<bankAccountId;
        return oss.str();
    }
    static User deserialize(const string& line)
    {
        User u;
        stringstream ss(line);
        string tok;
        getline(ss,tok,','); u.id = stoi(tok);
        getline(ss,tok,','); u.username = tok;
        getline(ss,tok,','); u.password = tok;
        getline(ss,tok,','); u.role = Role(stoi(tok));
        getline(ss,tok); u.bankAccountId = stoi(tok);
        return u;
    }
};
struct Item
{
    int id;
    int sellerId;
    string name;
    int stock;
    double price;
    int totalSold;
    Item():id(0),sellerId(0),stock(0),price(0),totalSold(0){}
    string serialize() const
    {
        ostringstream oss;
        oss<<id<<","<<sellerId<<","<<name<<","<<stock<<","<<fixed<<setprecision(2)<<price<<","<<totalSold;
        return oss.str();
    }
    static Item deserialize(const string& line)
    {
        Item it;
        stringstream ss(line);
        string tok;
        getline(ss,tok,','); it.id = stoi(tok);
        getline(ss,tok,','); it.sellerId = stoi(tok);
        getline(ss,tok,','); it.name = tok;
        getline(ss,tok,','); it.stock = stoi(tok);
        getline(ss,tok,','); it.price = stod(tok);
        getline(ss,tok); it.totalSold = stoi(tok);
        return it;
    }
};
struct Transaction
{
    int id;
    int buyerId;
    int sellerId;
    int itemId;
    int qty;
    double amount;
    string timeIso;
    TxStatus status;
    Transaction():id(0),buyerId(0),sellerId(0),itemId(0),qty(0),amount(0),status(STATUS_PAID),timeIso(now_iso()){}
    string serialize() const
    {
        ostringstream oss;
        oss<<id<<","<<buyerId<<","<<sellerId<<","<<itemId<<","<<qty<<","<<fixed<<setprecision(2)<<amount<<","<<timeIso<<","<<int(status);
        return oss.str();
    }
    static Transaction deserialize(const string& line)
    {
        Transaction t;
        stringstream ss(line);
        string tok;
        getline(ss,tok,','); t.id = stoi(tok);
        getline(ss,tok,','); t.buyerId = stoi(tok);
        getline(ss,tok,','); t.sellerId = stoi(tok);
        getline(ss,tok,','); t.itemId = stoi(tok);
        getline(ss,tok,','); t.qty = stoi(tok);
        getline(ss,tok,','); t.amount = stod(tok);
        getline(ss,tok,','); t.timeIso = tok;
        getline(ss,tok); t.status = TxStatus(stoi(tok));
        return t;
    }
};
#endif
