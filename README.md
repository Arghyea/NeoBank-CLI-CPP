# NeoBank-CLI-CPP

This Bank Account Simulation project is a console-based application that allows users to manage bank accounts and perform various banking operations. It supports both Savings and Checking accounts, with features such as deposits, withdrawals, transaction history tracking, interest calculations, and monthly account processing.
Key Features:

Account Creation:
    Users can create Savings or Checking accounts by setting a unique PIN for each account.

Deposits and Withdrawals:
    Savings accounts become inactive if the balance drops below $25.
    Checking accounts support overdrafts with a $15 fee per overdraft.

Transaction History:
    Maintains a record of deposits, withdrawals, interest earned, and service charges for each account.

Monthly Processing:
    Applies service charges and monthly interest.
    Resets the monthly transaction counts.

Authentication:
    Secure login using account number and PIN.

Data Persistence:
    Account information and transaction history are saved to a file and reloaded when the program starts, ensuring data continuity between sessions.

Functionalities:

Create Account
Deposit Funds
Withdraw Funds
View Transactions
Monthly Processing
Logout Functionality


How to Run:

Compile the code using g++:

    g++ bank_simulation.cpp -o bank_simulation

Execute the compiled program:

    ./bank_simulation   # Linux/Mac  
    bank_simulation.exe  # Windows  

