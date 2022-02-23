#!/bin/bash
rm -f l.test1.log
AUTO=1 SILENT=1 make run &
cat guest.out >> l.test1.log &
sleep 0.5
printf "ab" >> guest.in
sleep 2
printf "\001x" >> guest.in
sleep 0.5
