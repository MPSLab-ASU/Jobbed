#!/bin/bash
rm -f l.test1.log
make run-silent &
cat guest.out >> l.test1.log &
sleep 0.5
printf "ab" >> guest.in
sleep 1
printf "\001x" >> guest.in
sleep 0.5
