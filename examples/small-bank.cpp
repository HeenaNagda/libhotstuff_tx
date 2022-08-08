/**
 * Author: Heena
 * 
 */
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <limits>
#include "salticidae/util.h"

#define UINT64_MAX std::numeric_limits<std::uint64_t>::max()

class SmallBank{
private:
    uint64_t n_users;
    std::vector<uint64_t> checking_accounts;
    std::vector<uint64_t> saving_accounts;
public:
    SmallBank(uint64_t n_users);
    void transaction_savings(uint64_t user_id, uint64_t amount);
    void deposit_checking(uint64_t user_id, uint64_t amount);
    void send_payment(uint64_t from_user_id, uint64_t to_user_id, uint64_t amount);
    void write_check(uint64_t user_id, uint64_t amount);
    void amalgamate(uint64_t user_id);
    void split_transaction(std::vector<std::pair<uint64_t, uint64_t>> payors, std::vector<uint64_t> party);
    std::pair<uint64_t,uint64_t> query(uint64_t user_id);
};

SmallBank::SmallBank(uint64_t n_users){
    this->n_users = n_users;
    
    /* initialize random seed: */
    srand (time(NULL));
    uint64_t const max_amount = 10000;

    /* Initialize accounts with random amount */
    for(uint64_t user_id=0; user_id<n_users; user_id++){
        checking_accounts.push_back(rand()%max_amount);
        saving_accounts.push_back(rand()%max_amount);
    }
}

void SmallBank::transaction_savings(uint64_t user_id, uint64_t amount){
    if(UINT64_MAX-saving_accounts[user_id] < amount){
        /* If amount is exceded to the maximum limit of uint64_t : discart transaction */
        return;
    }
    saving_accounts[user_id] += amount;
}

void SmallBank::deposit_checking(uint64_t user_id, uint64_t amount){
    if(UINT64_MAX-checking_accounts[user_id] < amount){
        /* If amount is exceded to the maximum limit of uint64_t : discart transaction */
        return;
    }
    checking_accounts[user_id] += amount;
}

void SmallBank::send_payment(uint64_t from_user_id, uint64_t to_user_id, uint64_t amount){
    if(checking_accounts[from_user_id]<amount
        || UINT64_MAX-checking_accounts[to_user_id] < amount){
        /* If the from_user_id has not sufficient amount in the checking account OR */
        /* If to_user_id amount is exceded to the maximum limit of uint64_t : discart transaction */
        return;
    }
    checking_accounts[from_user_id] -= amount;
    checking_accounts[to_user_id] += amount;
}

void SmallBank::write_check(uint64_t user_id, uint64_t amount){
    if(checking_accounts[user_id]<amount){
        /* If the user has not sufficient amount in the checking account : discart transaction */
        return;
    }
    checking_accounts[user_id] -= amount;
}

void SmallBank::amalgamate(uint64_t user_id){
    if(UINT64_MAX-checking_accounts[user_id] < saving_accounts[user_id]){
        /* If amount is exceded to the maximum limit of uint64_t : discart transaction */
        return;
    }
    checking_accounts[user_id] += saving_accounts[user_id];
    saving_accounts[user_id] = 0;
}

void SmallBank::split_transaction(std::vector<std::pair<uint64_t, uint64_t>> payors, std::vector<uint64_t> party){
    auto party_size = party.size();
    auto amount=0;
    for(auto payor: payors){
        amount += payor.second;
    }
    auto amount_to_pay = amount/party.size();

    /* validity check */
    for(auto payor: payors){
        if(checking_accounts[payor.first]<payor.second){
            /* If the user has not sufficient amount in the checking account : discart transaction */
            return;
        }
    }
    for(auto user_id: party){
        if(UINT64_MAX-checking_accounts[user_id] < amount_to_pay){
            /* If amount is exceded to the maximum limit of uint64_t : discart transaction */
            return;
        }
    }

    /* Do transaction */
    for(auto payor: payors){
        checking_accounts[payor.first] += payor.second;
    }
    for(auto user_id: party){
        checking_accounts[user_id] -= amount_to_pay;
    }

}

std::pair<uint64_t,uint64_t> SmallBank::query(uint64_t user_id){
    return std::make_pair(checking_accounts[user_id], saving_accounts[user_id]);
}
 
