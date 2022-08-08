/**
 * Author: Heena
 * 
 */
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <limits>
#include <iostream>
#include <random>

#include "salticidae/util.h"


// #define UINT64_MAX std::numeric_limits<std::uint64_t>::max()

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
    void transaction_split(std::vector<std::pair<uint64_t, uint64_t>> payors, std::vector<uint64_t> party);
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

void SmallBank::transaction_split(std::vector<std::pair<uint64_t, uint64_t>> payors, std::vector<uint64_t> party){
    auto party_size = party.size();
    auto amount_payed=0;
    for(auto payor: payors){
        amount_payed += payor.second;
    }
    auto per_head_amount_to_pay = amount_payed/party.size();

    /* validity check */
    for(auto payor: payors){
        if(checking_accounts[payor.first]<payor.second){
            /* If the user has not sufficient amount in the checking account : discart transaction */
            return;
        }
    }
    for(auto user_id: party){
        if(UINT64_MAX-checking_accounts[user_id] < per_head_amount_to_pay){
            /* If amount is exceded to the maximum limit of uint64_t : discart transaction */
            return;
        }
    }

    /* Do transaction */
    for(auto payor: payors){
        checking_accounts[payor.first] += payor.second;
    }
    for(auto user_id: party){
        checking_accounts[user_id] -= per_head_amount_to_pay;
    }

}

std::pair<uint64_t,uint64_t> SmallBank::query(uint64_t user_id){
    return std::make_pair(checking_accounts[user_id], saving_accounts[user_id]);
}

class SmallBankManager{
private:
    uint64_t n_users;
    double prob_choose_mtx;                 /* Prob of choosing modifying transaction */

    std::default_random_engine tx_generator;
    std::bernoulli_distribution tx_distribution;

    std::default_random_engine mtx_generator;
    std::pair<uint64_t, uint64_t> random_transactions;
    std::uniform_int_distribution<uint64_t> mtx_distribution;

    std::default_random_engine user_generator;
    std::pair<uint64_t, uint64_t> random_users;
    std::uniform_int_distribution<uint64_t> user_distribution;


public:
    SmallBankManager(uint64_t n_users, double prob_choose_mtx);
    std::vector<uint64_t> get_next_transaction();
    std::pair<uint64_t, uint64_t> execute_transaction(uint64_t payload);
};

SmallBankManager::SmallBankManager(uint64_t n_users, double prob_choose_mtx){
    this->n_users = n_users;
    this->prob_choose_mtx = prob_choose_mtx;

    tx_distribution = std::bernoulli_distribution(prob_choose_mtx);

    random_transactions = std::make_pair(0,9);
    mtx_distribution = std::uniform_int_distribution<uint64_t>(
                                        random_transactions.first, 
                                        random_transactions.second);
    random_users = std::make_pair(10,20);
    user_distribution = std::uniform_int_distribution<uint64_t>(
                                        random_users.first, 
                                        random_users.second);
    
    int count=0;  // count number of trues
    for (int i=0; i<10000; ++i) if (tx_distribution(tx_generator)) ++count;
    printf("[Choosing mtx: %d] [Not Choosing mtx: %d]\n", count, 10000-count );

    for(int i=0; i<=9; i++){
        printf("[Iteration: %d] [Tx num: %ld]\n", i, mtx_distribution(mtx_generator));
    }
    for(int i=10; i<=20; i++){
        printf("[Iteration: %d] [User num: %ld]\n", i-10, user_distribution(user_generator));
    }
}

std::vector<uint64_t> SmallBankManager::get_next_transaction(){
    return std::vector<uint64_t>();
}

std::pair<uint64_t, uint64_t> SmallBankManager::execute_transaction(uint64_t payload){

}
 

/******************************** Test Small Bank /********************************/
int main(){
    // uint64_t n = 11;
    // SmallBank *bank = new SmallBank(n);

    // for(uint64_t i=0; i<n; i++){
    //     printf("[User: %ld] [C: %ld] [S: %ld]\n", i, bank->query(i).first, bank->query(i).second);
    // }

    // bank->transaction_savings(0, 10);
    // bank->deposit_checking(1,10);
    // bank->send_payment(2,3,10);
    // bank->write_check(4, 10);
    // bank->amalgamate(5);
    // std::vector<std::pair<uint64_t,uint64_t>> payors{std::make_pair(6,30), std::make_pair(7,20)};
    // std::vector<uint64_t> party{6,7,8,9,10};
    // bank->transaction_split(payors, party);

    // printf("\nAfter Transactions :\n\n");
    // for(uint64_t i=0; i<n; i++){
    //     printf("[User: %ld] [C: %ld] [S: %ld]\n", i, bank->query(i).first, bank->query(i).second);
    // }

    SmallBankManager *manager = new SmallBankManager(11, 0.9);
    manager->get_next_transaction();

    return 0;
}