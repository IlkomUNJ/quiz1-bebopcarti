#include "menu.h"
#include "datastore.h"
#include <iostream>
using namespace std;
int main()
{
    storeData.load_all();
    load_seed_if_empty();
    while(true)
    {
        cout<<"\nMain: 1-Register 2-Login 3-StoreAdmin 4-BankAdmin 5-MarkComplete 6-CancelTx 7-Exit\nChoice: ";
        int c = input_int();
        if(c==1) register_user();
        else if(c==2)
        {
            User* u = login_user();
            if(!u) continue;
            if(u->role==ROLE_BUYER) buyer_menu(u);
            else if(u->role==ROLE_SELLER) seller_menu(u);
            else cout<<"No role assigned\n";
        }
        else if(c==3) admin_store_menu();
        else if(c==4) admin_bank_menu();
        else if(c==5) mark_transaction_completed();
        else if(c==6) cancel_transaction();
        else if(c==7) break;
        else cout<<"Invalid\n";
    }
    return 0;
}
