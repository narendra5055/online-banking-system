#include <iostream> // For input/output operations
#include <string>   // For string manipulation
#include <vector>   // For dynamic arrays (transaction history)
#include <map>      // For key-value pairs (customers, accounts)
#include <memory>   // For smart pointers (shared_ptr for memory management)
#include <stdexcept> // For standard exceptions (e.g., ValueError)
#include <iomanip>  // For output formatting (e.g., setprecision)
#include <chrono>   // For date and time
#include <ctime>    // For time_t and tm structures
#include <sstream>  // For string stream operations

// Use the std namespace as requested
using namespace std;

// --- Forward Declarations ---
// Forward declarations are needed because Bank and Customer classes
// will refer to each other or to Account before their full definitions.
class Account;
class SavingsAccount;
class CheckingAccount;
class Customer;
class Bank;

// --- Helper Function for Current Time (for Transaction History) ---
string getCurrentDateTime() {
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);
    tm* local_tm = localtime(&now_time); // Use localtime for local time

    stringstream ss;
    ss << put_time(local_tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// --- DSA: Transaction Struct ---
// Represents a single transaction record.
struct Transaction {
    string type;
    double amount;
    string date;
    double newBalance;

    // Constructor for easy initialization
    Transaction(string type, double amount, double newBalance)
        : type(type), amount(amount), date(getCurrentDateTime()), newBalance(newBalance) {}

    // Method to print transaction details
    void print() const {
        cout << "  - " << date << " | Type: " << type
                  << " | Amount: $" << fixed << setprecision(2) << amount
                  << " | New Balance: $" << newBalance << endl;
    }
};

// --- OOP Classes ---

// Base class for all bank accounts.
// Demonstrates encapsulation and common attributes.
class Account {
protected: // Protected members are accessible by derived classes
    string _accountNumber;
    string _ownerName;
    double _balance;
    vector<Transaction> _transactions; // DSA: Vector to store transaction history

public:
    // Constructor
    Account(const string& accountNumber, const string& ownerName, double initialBalance = 0.0)
        : _accountNumber(accountNumber), _ownerName(ownerName), _balance(initialBalance) {
        if (accountNumber.empty()) {
            throw invalid_argument("Account number cannot be empty.");
        }
        if (ownerName.empty()) {
            throw invalid_argument("Owner name cannot be empty.");
        }
        if (initialBalance < 0) {
            throw invalid_argument("Initial balance cannot be negative.");
        }
    }

    // Virtual destructor to ensure proper cleanup of derived classes
    virtual ~Account() = default;

    // Getter methods
    string getAccountNumber() const { return _accountNumber; }
    string getOwnerName() const { return _ownerName; }
    double getBalance() const { return _balance; }

    // Deposits money into the account.
    // Adds a transaction record.
    bool deposit(double amount) {
        if (amount <= 0) {
            cout << "Deposit amount must be a positive number." << endl;
            return false;
        }
        _balance += amount;
        _transactions.emplace_back("Deposit", amount, _balance); // Add transaction
        cout << "Deposited $" << fixed << setprecision(2) << amount
                  << " into account " << _accountNumber << ". New balance: $" << _balance << endl;
        return true;
    }

    // Withdraws money from the account.
    // Checks for sufficient funds. Adds a transaction record.
    // This method is virtual to allow overriding in subclasses for specific rules (Polymorphism).
    virtual bool withdraw(double amount) {
        if (amount <= 0) {
            cout << "Withdrawal amount must be a positive number." << endl;
            return false;
        }
        if (_balance < amount) {
            cout << "Insufficient funds. Current balance: $" << fixed << setprecision(2) << _balance
                      << ". Attempted withdrawal: $" << amount << endl;
            return false;
        }
        _balance -= amount;
        _transactions.emplace_back("Withdrawal", amount, _balance); // Add transaction
        cout << "Withdrew $" << fixed << setprecision(2) << amount
                  << " from account " << _accountNumber << ". New balance: $" << _balance << endl;
        return true;
    }

    // Returns the list of transactions for this account.
    const vector<Transaction>& getTransactionHistory() const {
        return _transactions;
    }

    // Virtual method to print account details (Polymorphism)
    virtual void printDetails() const {
        cout << "Account Number: " << _accountNumber
                  << ", Owner: " << _ownerName
                  << ", Balance: $" << fixed << setprecision(2) << _balance;
    }
};

// Represents a savings account with an interest rate.
// Inherits from Account.
class SavingsAccount : public Account {
private:
    double _interestRate;

public:
    // Constructor
    SavingsAccount(const string& accountNumber, const string& ownerName,
                   double initialBalance = 0.0, double interestRate = 0.01)
        : Account(accountNumber, ownerName, initialBalance), _interestRate(interestRate) {
        if (interestRate < 0 || interestRate > 1) {
            throw invalid_argument("Interest rate must be between 0 and 1 (e.g., 0.01 for 1%).");
        }
    }

    // Applies interest to the account balance.
    void applyInterest() {
        double interestAmount = _balance * _interestRate;
        _balance += interestAmount;
        _transactions.emplace_back("Interest Applied", interestAmount, _balance); // Add transaction
        cout << "Interest of $" << fixed << setprecision(2) << interestAmount
                  << " applied to savings account " << _accountNumber << ". "
                  << "New balance: $" << _balance << endl;
    }

    // Overrides the printDetails method for SavingsAccount specific information.
    void printDetails() const override {
        cout << "Savings Account Number: " << _accountNumber
                  << ", Owner: " << _ownerName
                  << ", Balance: $" << fixed << setprecision(2) << _balance
                  << ", Interest Rate: " << _interestRate * 100 << "%";
    }
};

// Represents a checking account with an optional overdraft limit.
// Inherits from Account.
class CheckingAccount : public Account {
private:
    double _overdraftLimit;

public:
    // Constructor
    CheckingAccount(const string& accountNumber, const string& ownerName,
                    double initialBalance = 0.0, double overdraftLimit = 0.0)
        : Account(accountNumber, ownerName, initialBalance), _overdraftLimit(overdraftLimit) {
        if (overdraftLimit < 0) {
            throw invalid_argument("Overdraft limit cannot be negative.");
        }
    }

    // Overrides the withdraw method to include overdraft logic (Polymorphism).
    bool withdraw(double amount) override {
        if (amount <= 0) {
            cout << "Withdrawal amount must be a positive number." << endl;
            return false;
        }

        if (_balance + _overdraftLimit < amount) {
            cout << "Withdrawal denied. Exceeds overdraft limit of $" << fixed << setprecision(2) << _overdraftLimit
                      << ". Current balance: $" << _balance << ". Attempted withdrawal: $" << amount << endl;
            return false;
        }

        _balance -= amount;
        _transactions.emplace_back("Withdrawal", amount, _balance); // Add transaction
        cout << "Withdrew $" << fixed << setprecision(2) << amount
                  << " from checking account " << _accountNumber << ". New balance: $" << _balance << endl;
        return true;
    }

    // Overrides the printDetails method for CheckingAccount specific information.
    void printDetails() const override {
        cout << "Checking Account Number: " << _accountNumber
                  << ", Owner: " << _ownerName
                  << ", Balance: $" << fixed << setprecision(2) << _balance
                  << ", Overdraft Limit: $" << _overdraftLimit;
    }
};

// Represents a bank customer, holding their accounts.
class Customer {
private:
    string _customerId;
    string _name;
    string _address;
    // DSA: Map to store accounts by account number. Using shared_ptr for memory management.
    map<string, shared_ptr<Account>> _accounts;

public:
    // Constructor
    Customer(const string& customerId, const string& name, const string& address)
        : _customerId(customerId), _name(name), _address(address) {
        if (customerId.empty()) {
            throw invalid_argument("Customer ID cannot be empty.");
        }
        if (name.empty()) {
            throw invalid_argument("Customer name cannot be empty.");
        }
        if (address.empty()) {
            throw invalid_argument("Address cannot be empty.");
        }
    }

    // Getter methods
    string getCustomerId() const { return _customerId; }
    string getName() const { return _name; }
    string getAddress() const { return _address; }

    // Adds an account to the customer's portfolio.
    void addAccount(shared_ptr<Account> account) {
        if (!account) {
            throw invalid_argument("Account pointer cannot be null.");
        }
        _accounts[account->getAccountNumber()] = account; // DSA: Map insertion O(log N)
        cout << "Account " << account->getAccountNumber() << " added for customer " << _name << "." << endl;
    }

    // Retrieves an account by its number (DSA: Map lookup O(log N)).
    shared_ptr<Account> getAccount(const string& accountNumber) const {
        auto it = _accounts.find(accountNumber);
        if (it != _accounts.end()) {
            return it->second;
        }
        return nullptr; // Account not found
    }

    // Returns a vector of all accounts for this customer.
    vector<shared_ptr<Account>> getAllAccounts() const {
        vector<shared_ptr<Account>> allCustomerAccounts;
        for (const auto& pair : _accounts) {
            allCustomerAccounts.push_back(pair.second);
        }
        return allCustomerAccounts;
    }

    // Prints customer details.
    void printDetails() const {
        cout << "Customer ID: " << _customerId
                  << ", Name: " << _name
                  << ", Address: " << _address
                  << ", Accounts: " << _accounts.size();
    }
};

// Manages all customers and accounts in the banking system.
// Uses dictionaries (maps) for efficient storage and retrieval.
class Bank {
private:
    string _name;
    // DSA: Map to store customers by customer_id. Using shared_ptr for memory management.
    map<string, shared_ptr<Customer>> _customers;
    // DSA: Map to store all accounts by account_number. Using shared_ptr for memory management.
    map<string, shared_ptr<Account>> _accounts;

    // Simple counter for generating unique IDs (for demonstration)
    long long _nextCustomerId = 1000;
    long long _nextAccountNumber = 100000;

public:
    // Constructor
    Bank(const string& name) : _name(name) {}

    // Public getter for the bank's name
    string getName() const { return _name; }

    // Creates and adds a new customer to the bank.
    shared_ptr<Customer> addCustomer(const string& name, const string& address) {
        string customerId = "C" + to_string(_nextCustomerId++); // Generate unique ID
        auto customer = make_shared<Customer>(customerId, name, address);
        _customers[customerId] = customer; // DSA: Map insertion O(log N)
        cout << "Customer '" << name << "' added with ID: " << customerId << endl;
        return customer;
    }

    // Retrieves a customer by their ID (DSA: Map lookup O(log N)).
    shared_ptr<Customer> getCustomer(const string& customerId) const {
        auto it = _customers.find(customerId);
        if (it != _customers.end()) {
            return it->second;
        }
        return nullptr; // Customer not found
    }

    // Creates a new account (Savings or Checking) for a given customer.
    shared_ptr<Account> createAccount(const string& customerId, const string& accountType,
                                           double initialBalance = 0.0,
                                           double interestRate = 0.01, double overdraftLimit = 0.0) {
        shared_ptr<Customer> customer = getCustomer(customerId);
        if (!customer) {
            cout << "Error: Customer with ID " << customerId << " not found." << endl;
            return nullptr;
        }

        string accountNumber = "ACC" + to_string(_nextAccountNumber++); // Generate unique account number
        shared_ptr<Account> account = nullptr;

        if (accountType == "savings") {
            account = make_shared<SavingsAccount>(accountNumber, customer->getName(), initialBalance, interestRate);
        } else if (accountType == "checking") {
            account = make_shared<CheckingAccount>(accountNumber, customer->getName(), initialBalance, overdraftLimit);
        } else {
            cout << "Invalid account type. Choose 'savings' or 'checking'." << endl;
            return nullptr;
        }

        customer->addAccount(account);
        _accounts[accountNumber] = account; // DSA: Map insertion O(log N)
        cout << "Successfully created a " << accountType << " account for " << customer->getName()
                  << " (ID: " << customerId << "). Account Number: " << accountNumber << endl;
        return account;
    }

    // Retrieves an account by its number (DSA: Map lookup O(log N)).
    shared_ptr<Account> getAccount(const string& accountNumber) const {
        auto it = _accounts.find(accountNumber);
        if (it != _accounts.end()) {
            return it->second;
        }
        return nullptr; // Account not found
    }

    // Transfers funds between two accounts.
    bool transferFunds(const string& fromAccountNum, const string& toAccountNum, double amount) {
        shared_ptr<Account> fromAccount = getAccount(fromAccountNum);
        shared_ptr<Account> toAccount = getAccount(toAccountNum);

        if (!fromAccount) {
            cout << "Error: Source account " << fromAccountNum << " not found." << endl;
            return false;
        }
        if (!toAccount) {
            cout << "Error: Destination account " << toAccountNum << " not found." << endl;
            return false;
        }
        if (fromAccountNum == toAccountNum) {
            cout << "Error: Cannot transfer to the same account." << endl;
            return false;
        }
        if (amount <= 0) {
            cout << "Transfer amount must be a positive number." << endl;
            return false;
        }

        if (fromAccount->withdraw(amount)) { // Use the virtual withdraw method
            toAccount->deposit(amount);      // Use the deposit method
            cout << "Successfully transferred $" << fixed << setprecision(2) << amount
                      << " from " << fromAccountNum << " to " << toAccountNum << "." << endl;
            return true;
        } else {
            cout << "Transfer failed due to insufficient funds or other withdrawal error." << endl;
            return false;
        }
    }

    // Displays details of all customers.
    void displayAllCustomers() const {
        if (_customers.empty()) {
            cout << "No customers in the bank yet." << endl;
            return;
        }
        cout << "\n--- All Customers ---" << endl;
        for (const auto& pair : _customers) {
            pair.second->printDetails();
            cout << endl;
            for (const auto& account : pair.second->getAllAccounts()) {
                cout << "  - ";
                account->printDetails();
                cout << endl;
            }
        }
        cout << "---------------------\n" << endl;
    }

    // Displays details of all accounts.
    void displayAllAccounts() const {
        if (_accounts.empty()) {
            cout << "No accounts in the bank yet." << endl;
            return;
        }
        cout << "\n--- All Accounts ---" << endl;
        for (const auto& pair : _accounts) {
            pair.second->printDetails();
            cout << endl;
        }
        cout << "--------------------\n" << endl;
    }
};

// --- Simulation / Usage Example ---

int main() {
    // Set output precision for currency
    cout << fixed << setprecision(2);

    Bank myBank("Global Bank Inc.");

    cout << "Welcome to " << myBank.getName() << "!" << endl; // Changed to use getName()

    // --- Create Customers ---
    cout << "\n--- Creating Customers ---" << endl;
    shared_ptr<Customer> customer1 = myBank.addCustomer("Alice Smith", "123 Main St, Anytown");
    shared_ptr<Customer> customer2 = myBank.addCustomer("Bob Johnson", "456 Oak Ave, Otherville");

    // --- Create Accounts ---
    cout << "\n--- Creating Accounts ---" << endl;
    shared_ptr<Account> acc1_savings = myBank.createAccount(customer1->getCustomerId(), "savings", 1000.0, 0.015);
    shared_ptr<Account> acc1_checking = myBank.createAccount(customer1->getCustomerId(), "checking", 500.0, 0.0, 200.0);
    shared_ptr<Account> acc2_savings = myBank.createAccount(customer2->getCustomerId(), "savings", 2500.0);
    shared_ptr<Account> acc2_checking = myBank.createAccount(customer2->getCustomerId(), "checking", 100.0, 0.0, 500.0);

    // Display initial state
    myBank.displayAllCustomers();
    myBank.displayAllAccounts();

    // --- Perform Operations ---
    cout << "\n--- Performing Operations ---" << endl;

    if (acc1_savings) {
        cout << "\n--- Operations on " << acc1_savings->getAccountNumber() << " (Alice's Savings) ---" << endl;
        acc1_savings->deposit(200.0);
        acc1_savings->withdraw(50.0);
        // Downcast to SavingsAccount to call applyInterest (safe because we know its type)
        static_pointer_cast<SavingsAccount>(acc1_savings)->applyInterest();
        cout << "Current balance for " << acc1_savings->getAccountNumber() << ": $" << acc1_savings->getBalance() << endl;
        cout << "Transaction History:" << endl;
        for (const auto& t : acc1_savings->getTransactionHistory()) {
            t.print();
        }
    }

    if (acc1_checking) {
        cout << "\n--- Operations on " << acc1_checking->getAccountNumber() << " (Alice's Checking) ---" << endl;
        acc1_checking->deposit(100.0);
        acc1_checking->withdraw(700.0); // Should use overdraft
        acc1_checking->withdraw(300.0); // Should fail (exceeds overdraft)
        cout << "Current balance for " << acc1_checking->getAccountNumber() << ": $" << acc1_checking->getBalance() << endl;
        cout << "Transaction History:" << endl;
        for (const auto& t : acc1_checking->getTransactionHistory()) {
            t.print();
        }
    }

    // --- Transfer Funds ---
    cout << "\n--- Transferring Funds ---" << endl;
    if (acc1_checking && acc2_savings) {
        myBank.transferFunds(acc1_checking->getAccountNumber(), acc2_savings->getAccountNumber(), 150.0);
        cout << "Alice's Checking balance after transfer: $" << acc1_checking->getBalance() << endl;
        cout << "Bob's Savings balance after transfer: $" << acc2_savings->getBalance() << endl;
    }

    // Attempt a transfer that should fail
    if (acc2_checking && acc1_savings) {
        myBank.transferFunds(acc2_checking->getAccountNumber(), acc1_savings->getAccountNumber(), 10000.0);
    }

    // Display final state
    myBank.displayAllCustomers();
    myBank.displayAllAccounts();

    return 0;
}
