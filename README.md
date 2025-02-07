# NeoBank-CLI-CPP

A console app for managing Savings and Checking accounts with features like:

    Account Creation: Unique PIN for each account.
    Transactions: Deposits, withdrawals ($15 overdraft fee for Checking).
    History: Tracks all transactions, interest, and charges.
    Monthly Processing: Applies charges, interest, and resets counts.
    Secure Login: PIN-based access.
    Data Persistence: Saves account info and history.



How to Run:

Compile the code using g++:

    g++ bank_simulation.cpp -o bank_simulation

Execute the compiled program:

    ./bank_simulation   # Linux/Mac  
    bank_simulation.exe  # Windows  

