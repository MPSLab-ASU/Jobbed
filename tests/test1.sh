#!/bin/bash
pushd ./
rm -f test1.log
make run &
cat guest.out >> test1.log &
sleep 1
printf "ab" >> guest.in
sleep 2
printf "\001x" >> guest.in
popd
