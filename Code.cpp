#include <iostream>
#include <vector>

struct BankAccount{ 
    int balance =0;
    int overdraft_limit = -500;

    void deposit(int amount){
        balance +=amount;
        std::cout << "deposited " << amount << ", balance is now" <<
            balance << "\n";
    }

    bool withdraw(int amount){
        if(balance - amount >= overdraft_limit){
            balance -= amount;
            std::cout << "withdrew " << amount << " , balance is now " << 
                balance << "\n";
            return true;
        }
        return false;
    }
};

struct Command{
    virtual void call() =0;
    virtual void undo() =0;
};

struct BankAccountCommand: Command{
    
    BankAccount& account;
    enum Action {deposit, withdraw} action;
    int amount;
    bool withdrawal_succeeded;

    BankAccountCommand(BankAccount& account, const Action action, const int amount)
        :account(account), action(action), amount(amount), withdrawal_succeeded(false){}

    void call()  override{
        switch(action){
            case deposit:
                account.deposit(amount);  
            break;
            case withdraw:
                withdrawal_succeeded = account.withdraw(amount); 
            break;
        }   
    }

    void undo() override{
        switch(action){
            case withdraw:
                if (withdrawal_succeeded)
                    account.deposit(amount);
            break;
            case deposit:
                account.withdraw(amount);
            break;
        }
    }
};

struct CompositeBankAccountCommand: std::vector<BankAccountCommand>, Command{
    CompositeBankAccountCommand(const std::initializer_list<value_type>& items): std::vector<BankAccountCommand>(items){}

    void call() override{
        /*for(auto&cmd: *this){
            cmd.call();
        }*/
    bool ok = true;
        for(auto& cmd: *this){
            if(ok){
                cmd.call();
                ok=cmd.withdrawal_succeeded;  //make sure
            }
            else{
                cmd.withdrawal_succeeded =false; //make sure
            }
        }
    }

    void undo() override{
        for(auto it=rbegin(); it != rend(); ++it)
            it->undo();
    }
};

struct MoneyTransferCommand: CompositeBankAccountCommand{
    MoneyTransferCommand(BankAccount& from, BankAccount& to, int amount):
    CompositeBankAccountCommand{
        BankAccountCommand{from , BankAccountCommand::withdraw, amount},
        BankAccountCommand{to , BankAccountCommand::deposit, amount},
    }{}
};
    
int main(){
    BankAccount ba;
    BankAccountCommand cmd{ba, BankAccountCommand::deposit, 100};
    cmd.call();
}
