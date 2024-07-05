#!/bin/bash

zs=(0.25 0.5)
for z in "${zs[@]}"
do
    java -cp .:/home/zhuoxh/waiting_room-master/fastutil-7.2.0.jar Main "$z" 100000 &
    wait
done