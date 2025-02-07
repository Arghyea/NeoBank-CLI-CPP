#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <fstream>
#include <map>
using namespace std;

struct Transaction {
    string date;
    string type;
    double amount;
    double balanceAfter;
};

class Account {
protected:
    int pin;
    double balance;
    int deposits;
    int withdrawals;
    double interestRate;
    double serviceCharges;
    bool status;
    vector<Transaction> transactions;

    string getCurrentDate() {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        stringstream ss;
        ss << 1900 + ltm->tm_year << "-" 
           << 1 + ltm->tm_mon << "-"
           << ltm->tm_mday;
        return ss.str();
    }

    void addTransaction(const string& type, double amount) {
        transactions.push_back({getCurrentDate(), type, amount, balance});
    }

public:
    Account(int pin) : pin(pin), balance(0), deposits(0), withdrawals(0),
                       interestRate(0.05), serviceCharges(0), status(true) {}

    virtual ~Account() {}

    int getPIN() const { return pin; }

    virtual void deposit(double amount) {
        if (amount <= 0) {
            cout << "Invalid deposit amount.\n";
            return;
        }
        balance += amount;
        deposits++;
        addTransaction("Deposit", amount);
    }

    virtual void withdraw(double amount) {
        if (amount <= 0) {
            cout << "Invalid withdrawal amount.\n";
            return;
        }
        if (balance >= amount) {
            balance -= amount;
            withdrawals++;
            addTransaction("Withdrawal", amount);
        } else {
            cout << "Insufficient funds!\n";
        }
    }

    virtual void calcInt() {
        double monthlyInterest = balance * (interestRate / 12);
        balance += monthlyInterest;
        addTransaction("Interest", monthlyInterest);
    }

    virtual void monthlyProc() {
        balance -= serviceCharges;
        if (serviceCharges > 0)
            addTransaction("Service Charge", -serviceCharges);
        
        calcInt();
        withdrawals = deposits = serviceCharges = 0;
    }

    void printTransactions() const {
        cout << "\nTransaction History:\n";
        for (const auto& t : transactions)
            cout << t.date << " - " << t.type << ": $" << t.amount 
                 << " | Balance: $" << t.balanceAfter << endl;
    }

    virtual string getType() const = 0;
    virtual void save(ofstream& file) const = 0;
    virtual void load(ifstream& file) = 0;

    double getBalance() const { return balance; }
};

class Savings : public Account {
public:
    Savings(int pin) : Account(pin) { interestRate = 0.05; }

    void withdraw(double amount) override {
        if (amount <= 0) {
            cout << "Invalid withdrawal amount.\n";
            return;
        }
        if (status && balance >= amount) {
            Account::withdraw(amount);
            if (balance < 25) status = false;
        } else {
            cout << "Account inactive or insufficient funds.\n";
        }
    }

    void deposit(double amount) override {
        Account::deposit(amount);
        if (balance >= 25) status = true;
    }

    void monthlyProc() override {
        if (withdrawals > 4)
            serviceCharges += (withdrawals - 4);
        Account::monthlyProc();
        if (balance < 25) status = false;
    }

    string getType() const override { return "Savings"; }

    void save(ofstream& file) const override {
        file << "S " << pin << " " << balance << " " << deposits << " "
             << withdrawals << " " << interestRate << " " << serviceCharges 
             << " " << status << endl;
        file << transactions.size() << endl;
        for (const auto& t : transactions)
            file << t.date << "\t" << t.type << "\t" 
                 << t.amount << "\t" << t.balanceAfter << endl;
    }

    void load(ifstream& file) override {
        file >> pin >> balance >> deposits >> withdrawals 
             >> interestRate >> serviceCharges >> status;
        int numTransactions;
        file >> numTransactions;
        transactions.clear();
        for (int i = 0; i < numTransactions; i++) {
            Transaction t;
            getline(file >> ws, t.date, '\t');
            getline(file >> ws, t.type, '\t');
            file >> t.amount >> t.balanceAfter;
            transactions.push_back(t);
        }
    }
};

class Checking : public Account {
public:
    Checking(int pin) : Account(pin) { interestRate = 0.03; }

    void withdraw(double amount) override {
        if (amount <= 0) {
            cout << "Invalid withdrawal amount.\n";
            return;
        }
        if (balance >= amount) {
            Account::withdraw(amount);
        } else {
            cout << "Overdraft! $15 fee charged.\n";
            balance -= 15;
            serviceCharges += 15;
            addTransaction("Overdraft Fee", -15);
        }
    }

    void monthlyProc() override {
        serviceCharges += 5 + (withdrawals * 0.10);
        Account::monthlyProc();
    }

    string getType() const override { return "Checking"; }

    void save(ofstream& file) const override {
        file << "C " << pin << " " << balance << " " << deposits << " "
             << withdrawals << " " << interestRate << " " << serviceCharges 
             << " " << status << endl;
        file << transactions.size() << endl;
        for (const auto& t : transactions)
            file << t.date << "\t" << t.type << "\t" 
                 << t.amount << "\t" << t.balanceAfter << endl;
    }

    void load(ifstream& file) override {
        file >> pin >> balance >> deposits >> withdrawals 
             >> interestRate >> serviceCharges >> status;
        int numTransactions;
        file >> numTransactions;
        transactions.clear();
        for (int i = 0; i < numTransactions; i++) {
            Transaction t;
            getline(file >> ws, t.date, '\t');
            getline(file >> ws, t.type, '\t');
            file >> t.amount >> t.balanceAfter;
            transactions.push_back(t);
        }
    }
};

class Bank {
private:
    map<int, Account*> accounts;
    int nextAccountNumber;
    const string filename = "bank_data.txt";

    void saveToFile() {
        ofstream file(filename);
        file << nextAccountNumber << endl;
        file << accounts.size() << endl;
        for (const auto& pair : accounts) {
            file << pair.first << " ";
            pair.second->save(file);
        }
    }

    void loadFromFile() {
        ifstream file(filename);
        if (!file) return;

        file >> nextAccountNumber;
        int numAccounts;
        file >> numAccounts;

        for (int i = 0; i < numAccounts; i++) {
            int accNumber;
            file >> accNumber;
            
            string type;
            file >> type;
            
            Account* acc = nullptr;
            if (type == "S") acc = new Savings(0);
            else if (type == "C") acc = new Checking(0);
            
            if (acc) {
                acc->load(file);
                accounts[accNumber] = acc;
            }
        }
    }

public:
    Bank() : nextAccountNumber(1) { loadFromFile(); }
    
    ~Bank() {
        saveToFile();
        for (auto& pair : accounts) delete pair.second;
    }

    int createAccount(const string& type, int pin) {
        Account* acc = nullptr;
        if (type == "Savings") acc = new Savings(pin);
        else if (type == "Checking") acc = new Checking(pin);
        
        if (acc) {
            int accNumber = nextAccountNumber++;
            accounts[accNumber] = acc;
            return accNumber;
        }
        return -1;
    }

    Account* getAccount(int accNumber) {
        auto it = accounts.find(accNumber);
        return (it != accounts.end()) ? it->second : nullptr;
    }

    bool authenticate(int accNumber, int pin) {
        Account* acc = getAccount(accNumber);
        return acc && acc->getPIN() == pin;
    }
};

void displayMainMenu() {
    cout << "\n===== Bank System =====\n"
         << "1. Create Account\n"
         << "2. Login\n"
         << "3. Exit\n"
         << "Choice: ";
}

void displayAccountMenu() {
    cout << "\n===== Account Menu =====\n"
         << "1. Deposit\n"
         << "2. Withdraw\n"
         << "3. View Transactions\n"
         << "4. Monthly Processing\n"
         << "5. Logout\n"
         << "Choice: ";
}

int main() {
    Bank bank;
    int currentAccount = 0;

    while (true) {
        if (currentAccount == 0) {
            displayMainMenu();
            int choice;
            cin >> choice;

            if (choice == 1) {
                string type;
                int pin;
                cout << "Account Type (Savings/Checking): ";
                cin >> type;
                cout << "Set PIN (numbers only): ";
                cin >> pin;
                int accNumber = bank.createAccount(type, pin);
                if (accNumber != -1)
                    cout << "Account created! Number: " << accNumber << endl;
                else
                    cout << "Invalid account type!\n";
            }
            else if (choice == 2) {
                int accNumber, pin;
                cout << "Account Number: ";
                cin >> accNumber;
                cout << "PIN: ";
                cin >> pin;
                if (bank.authenticate(accNumber, pin)) {
                    currentAccount = accNumber;
                    cout << "Login successful!\n";
                } else {
                    cout << "Invalid credentials!\n";
                }
            }
            else if (choice == 3) break;
        }
        else {
            Account* acc = bank.getAccount(currentAccount);
            displayAccountMenu();
            int choice;
            cin >> choice;

            switch (choice) {
                case 1: {
                    double amount;
                    cout << "Deposit Amount: $";
                    cin >> amount;
                    acc->deposit(amount);
                    break;
                }
                case 2: {
                    double amount;
                    cout << "Withdraw Amount: $";
                    cin >> amount;
                    acc->withdraw(amount);
                    break;
                }
                case 3:
                    acc->printTransactions();
                    break;
                case 4:
                    acc->monthlyProc();
                    cout << "Monthly processing completed.\n";
                    break;
                case 5:
                    currentAccount = 0;
                    cout << "Logged out.\n";
                    break;
                default:
                    cout << "Invalid choice!\n";
            }
        }
    }
    return 0;
}
