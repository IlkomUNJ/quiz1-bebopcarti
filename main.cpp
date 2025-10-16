#include <iostream>
#include <vector>
#include <string>
#include <limits> 
#include "bank_customer.h"
#include "buyer.h"
#include "seller.h"
#include "item.h"


using namespace std;

void showBankMenu(BankCustomer& customer);
void showLoggedInMenu(Buyer& buyer, vector<seller>& allSellers);
void registerNewUser(vector<BankCustomer>& customers, vector<Buyer>& buyers, vector<seller>& sellers);

int main(){
    vector<BankCustomer> bankCustomers;
    vector<Buyer> buyers;
    vector<seller> sellers;

    BankCustomer customer1(1, "Alice", 1500.0);
    bankCustomers.push_back(customer1);
    Buyer buyer1(1, "Alice", bankCustomers.at(0));
    buyers.push_back(buyer1);

    BankCustomer customer2(2, "Bob", 800.0);
    bankCustomers.push_back(customer2);
    Buyer buyer2(2, "Bob", bankCustomers.at(1));
    buyers.push_back(buyer2);
    seller seller2(buyer2, 1, "Bob's Gadgets");
    sellers.push_back(seller2);

    int choice = 0;
    while (choice != 3){
        cout << "==============================" << endl;
        cout << "   Welcome to the E-Market   " << endl;
        cout << "==============================" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "3. Exit" << endl;
        cout << "Please select an option: ";
        
        cin >> choice;

        if(cin.fail()){
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            continue; 
        }

        switch (choice){
            case 1:{ 
                cout << "\n--- Login ---" << endl;
                cout << "Enter your User ID: ";
                int loginId;
                cin >> loginId;

                bool loggedIn = false;
                for (Buyer& buyer : buyers){
                    if (buyer.getId() == loginId){
                        cout << "\nLogin successful! Welcome, " << buyer.getName() << "." << endl;
                        loggedIn = true;
                        showLoggedInMenu(buyer, sellers); 
                        break; 
                    }
                }

                if (!loggedIn){
                    cout << "Login failed. User ID not found." << endl;
                }
                break;
            }
            case 2:{ 
                cout << "\n--- New User Registration ---" << endl;
                registerNewUser(bankCustomers, buyers, sellers);
                break;
            }
            case 3:{ 
                cout << "Thank you for using the E-Market. Exiting." << endl;
                break;
            }
            default:{
                cout << "Invalid option. Please try again." << endl;
                break;
            }
        }
        cout << endl; 
    }

    return 0; 
}

void registerNewUser(vector<BankCustomer>& customers, vector<Buyer>& buyers, vector<seller>& sellers){
    string name;
    double initialDeposit;

    cout << "Enter your full name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
    getline(cin, name);

    cout << "To register, you must create a bank account." << endl;
    cout << "Enter initial deposit amount: $";
    cin >> initialDeposit;

    int newId = customers.size() + 1;

    BankCustomer newCustomer(newId, name, initialDeposit);
    customers.push_back(newCustomer);

    Buyer newBuyer(newId, name, customers.back());
    buyers.push_back(newBuyer);
    
    cout << "\nCongratulations, " << name << "! Your account has been created." << endl;
    cout << "Your new User ID is: " << newId << endl;

    char choice;
    cout << "Would you like to register as a Seller as well? (y/n): ";
    cin >> choice;

    if (choice == 'y' || choice == 'Y'){
        string storeName;
        cout << "Enter your store name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, storeName);
        
        int newSellerId = sellers.size() + 1;
        seller newSeller(newBuyer, newSellerId, storeName);
        sellers.push_back(newSeller);
        
        cout << "Seller account '" << storeName << "' created successfully!" << endl;
    }
}


void showLoggedInMenu(Buyer& buyer, vector<seller>& allSellers) {
    int choice = 0;
    while (choice != 4){
        cout << "\n--- User Menu ---" << endl;
        cout << "1. Banking Functions" << endl;
        cout << "2. Upgrade Account to Seller" << endl;
        cout << "3. Seller Functions (Add Item to Inventory)" << endl;
        cout << "4. Logout" << endl;
        cout << "Please select an option: ";
        cin >> choice;
        
        if(cin.fail()){
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice){
            case 1:
                showBankMenu(buyer.getAccount());
                break;
            case 2:{ 
                bool isAlreadySeller = false;
                for (const auto& s : allSellers) {
                    if (s.getId() == buyer.getId()){
                        isAlreadySeller = true;
                        break;
                    }
                }

                if (isAlreadySeller){
                    cout << "You are already registered as a seller." << endl;
                } else{
                    string storeName;
                    cout << "Enter your new store name: ";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    getline(cin, storeName);
                    
                    int newSellerId = allSellers.size() + 1;
                    seller newSeller(buyer, newSellerId, storeName);
                    allSellers.push_back(newSeller);
                    cout << "Success! You are now also a seller." << endl;
                }
                break;
            }
            case 3:{ 
                seller* currentSeller = nullptr;
                for (auto& s : allSellers){
                    if (s.getId() == buyer.getId()){
                        currentSeller = &s;
                        break;
                    }
                }
                
                if (currentSeller != nullptr){
                    cout << "\n--- Add New Item to Inventory ---" << endl;
                    int itemId, quantity;
                    string itemName;
                    double price;
                    
                    cout << "Enter Item ID: ";
                    cin >> itemId;
                    cout << "Enter Item Name: ";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    getline(cin, itemName);
                    cout << "Enter Quantity: ";
                    cin >> quantity;
                    cout << "Enter Price: $";
                    cin >> price;
                    
                    currentSeller->addNewItem(itemId, itemName, quantity, price);
                    cout << "'" << itemName << "' has been added to your inventory." << endl;
                } else {
                    cout << "Access denied. You must be a seller to use this function." << endl;
                }
                break;
            }
            case 4:
                cout << "Logging out..." << endl;
                break;
            default:
                cout << "Invalid option. Please try again." << endl;
                break;
        }
    }
}

void showBankMenu(BankCustomer& customer){
    int choice = 0;
    while (choice != 4) {
        cout << "\n--- Bank Menu ---" << endl;
        cout << "Account Holder: " << customer.getName() << endl;
        cout << "1. Check Balance" << endl;
        cout << "2. Deposit" << endl;
        cout << "3. Withdraw" << endl;
        cout << "4. Return to Main Menu" << endl;
        cout << "Please select an option: ";
        cin >> choice;

        if(cin.fail()){
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        switch (choice){
            case 1:
                cout << "\n--- Account Status ---" << endl;
                customer.printInfo();
                break;
            case 2: {
                double amount;
                cout << "Enter amount to deposit: $";
                cin >> amount;
                customer.addBalance(amount);
                cout << "Deposit successful. New balance: $" << customer.getBalance() << endl;
                break;
            }
            case 3:{
                double amount;
                cout << "Enter amount to withdraw: $";
                cin >> amount;
                customer.withdrawBalance(amount); // This method already handles printing errors
                cout << "Current balance: $" << customer.getBalance() << endl;
                break;
            }
            case 4:
                cout << "Returning to user menu..." << endl;
                break;
            default:
                cout << "Invalid option. Please try again." << endl;
                break;
        }
    }
}