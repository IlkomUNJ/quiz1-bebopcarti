#ifndef MENU_H
#define MENU_H
#include "datastore.h"
#include "entities.h"
#include <string>
using namespace std;
int input_int();
double input_double();
void register_user();
User* login_user();
void create_bank_account(User* u);
void topup(User* u);
void withdraw(User* u);
void list_orders(User* u);
void filter_orders(User* u);
void spending_last_k_days(User* u);
void buyer_menu(User* u);
void seller_add_item(User* u);
void seller_manage_item(User* u);
void seller_discover_popular(User* u);
void seller_loyal_customers(User* u);
void seller_menu(User* u);
void store_list_latest_k_days();
void store_list_paid_not_completed();
void store_most_m_frequent_items();
void store_most_active_buyers_per_day();
void store_most_active_sellers_per_day();
void bank_list_week_backwards();
void bank_list_all_customers();
void bank_list_dormant_accounts();
void bank_top_n_users_today();
void admin_store_menu();
void admin_bank_menu();
void mark_transaction_completed();
void cancel_transaction();
void load_seed_if_empty();
#endif
