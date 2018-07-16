#!/bin/bash

set -x

nums="1 2 3 4 5"

createuser(){
cleos create account eosio transfergame EOS7ijWCBmoXBi3CgtK7DJxentZZeTkeUnaSDvyro9dq7Sd1C3dC4
cleos push action eosio.token transfer '[ "user", "transfergame", "1000000.0000 EOS", "m" ]' -p user@active

for i in $nums; do
	for j in $nums; do
		name=gameplayer$i$j
		echo $name
		cleos create account eosio $name EOS7ijWCBmoXBi3CgtK7DJxentZZeTkeUnaSDvyro9dq7Sd1C3dC4
		cleos push action eosio.token transfer '[ "user", "'$name'", "1000000.0000 EOS", "m" ]' -p user@active
	done
done
}

createuser
