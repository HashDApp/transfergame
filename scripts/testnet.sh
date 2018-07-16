#!/bin/bash

set -x

RPC="https://jungle.eosio.cr"

CONTRACT="transfergame"

numi="1 2"
numj="1 2 3 4 5"

cleos -u $RPC set contract $CONTRACT ../$CONTRACT -p $CONTRACT@active
cleos -u $RPC set account permission $CONTRACT active '{"threshold": 1,"keys": [{"key": "EOS6Q6vkzi1xeY4XqPxryfW4YAQAeVg3gJSt7tYGTbRq2SpL5Jvt5","weight": 1}],"accounts": [{"permission":{"actor":"'$CONTRACT'","permission":"eosio.code"},"weight":1}]}' owner -p $CONTRACT

play(){
for i in $numi; do
	for j in $numj; do
		name=gameplayer$i$j
		echo $name
		cleos -u $RPC push action eosio.token transfer '[ "'$name'", "'$CONTRACT'", "0.0001 EOS", "m" ]' -p $name@active
	done
done

cleos -u $RPC push action eosio.token transfer '[ "gameplayer11", "'$CONTRACT'", "10.0000 EOS", "m" ]' -p gameplayer11@active
}


play

cleos -u $RPC get table $CONTRACT $CONTRACT account

cleos -u $RPC get table $CONTRACT $CONTRACT counter

