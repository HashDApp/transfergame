/**
 *  @transfergame
 */

#include <eosiolib/transaction.hpp>
#include <eosio.token/eosio.token.hpp>
#include <eosiolib/crypto.h>

using namespace eosio;
using namespace std;

namespace transfergame {

    //@abi table counter i64
    struct counter {
        uint64_t id;
        uint64_t total_round = 0;
        asset total_balance = asset(0, S(4, EOS));
        uint64_t trans_cnt = 0;
        asset round_balance = asset(0, S(4, EOS));

        uint64_t primary_key() const {
            return id;
        }

        EOSLIB_SERIALIZE(counter, (id) (total_round) (total_balance) (trans_cnt) (round_balance) )
    };

    typedef eosio::multi_index < N(counter), counter > counter_index;

    //@abi table account i64
    struct account {
        uint64_t id;
        account_name owner;
        asset eos_balance = asset(0, S(4, EOS));

        uint64_t primary_key() const {
            return id;
        }

        EOSLIB_SERIALIZE(account, (id) (owner) (eos_balance) )
    };

    typedef eosio::multi_index < N(account), account > account_index;

    template < typename T >
    void apply_transfer(uint64_t receiver, const T & transfer)
    {
        const auto self = receiver;
        auto from = transfer.from;
        if (from == self) {
            return;
        }

        auto to = transfer.to;
        if (to != self) {
            return;
        }

        auto quantity = transfer.quantity;
        if (quantity.symbol != S(4, EOS)) {
            return;
        }

        uint64_t tc;

        counter_index counter(self, self);
        auto citr = counter.find(0);
        if (citr == counter.end()) {
            citr = counter.emplace(from, [&](auto & cnt) {
                            cnt.id = 0;
                        });
        }
        counter.modify(citr, from, [&](auto & cnt) {
                cnt.trans_cnt += 1;
                cnt.total_balance += quantity;
                cnt.round_balance += quantity;
                tc = cnt.trans_cnt;
            }
        );

        account_index accounts(self, self);
        auto itr = accounts.find(tc);
        if (itr == accounts.end()) {
            itr = accounts.emplace(from, [&](auto & acnt) {
                           acnt.id = tc;
                });
        }
        accounts.modify(itr, from, [&](auto & acnt) {
                acnt.owner = from;
                acnt.eos_balance = quantity;
            }
        );

        if (tc < 11) {
            return;
        }

        asset round_balance;
        counter.modify(citr, from, [&](auto & cnt) {
                cnt.trans_cnt = 0;
                cnt.total_round += 1;
                round_balance = cnt.round_balance;
                cnt.round_balance = asset(0, S(4, EOS));
            }
        );

        string factors = "";
        itr = accounts.begin();
        while (itr != accounts.end()) {
            auto e = itr;
            itr++;
            factors.append(name{e->owner}.to_string());
        }
        factors.append(to_string(current_time()));
        factors.append(to_string(tapos_block_num()));
        checksum256 calc_hash;
        sha256((char *)factors.c_str(), factors.length(), &calc_hash);
        const uint64_t *p64 = reinterpret_cast<const uint64_t *>(&calc_hash);
        uint64_t w = p64[0] % round_balance.amount + 1;

        account_name winner = self;
        uint64_t weight_offset = 1;
        itr = accounts.begin();
        while (itr != accounts.end()) {
            auto e = itr;
            itr++;
            if (weight_offset <= w && w < (weight_offset + e->eos_balance.amount)) {
                winner = e->owner;
            }
            weight_offset += e->eos_balance.amount;

            accounts.erase(e);
        }

        action(
            permission_level{ self, N(active) },
            N(eosio.token), N(transfer),
            std::make_tuple(self, winner,
                    round_balance - asset(1, S(4, EOS)),// keep 0.0001 EOS
                    std::string("WINNER"))
         ).send();
    }

}

using namespace transfergame;

extern "C" {

    /// The apply method implements the dispatch of events to this contract
    void apply(uint64_t receiver, uint64_t code, uint64_t action) {
        if (code == N(eosio.token) && action == N(transfer)) {
            apply_transfer(receiver,
                           unpack_action_data<eosio::token::transfer_args >());
        }
    }

}

