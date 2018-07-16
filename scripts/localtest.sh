#!/bin/bash

set -x

nums="1 2 3 4 5"

play(){
for i in $nums; do
	for j in $nums; do
		name=gameplayer$i$j
		echo $name
		cleos push action eosio.token transfer '[ "'$name'", "transfergame", "0.0001 EOS", "m" ]' -p $name@active
	done
done

cleos push action eosio.token transfer '[ "gameplayer55", "transfergame", "10.0000 EOS", "m" ]' -p gameplayer55@active
}

cleos set contract transfergame ../transfergame -p transfergame@active
cleos set account permission transfergame active '{"threshold": 1,"keys": [{"key": "EOS7ijWCBmoXBi3CgtK7DJxentZZeTkeUnaSDvyro9dq7Sd1C3dC4","weight": 1}],"accounts": [{"permission":{"actor":"transfergame","permission":"eosio.code"},"weight":1}]}' owner -p transfergame

play

cleos get table transfergame transfergame account
cleos get table transfergame transfergame counter

