#!/bin/bash


resSizes=(100000 1000000)


for resSize in "${resSizes[@]}"
do
    echo "Testing with resSize: $resSize"

    
    for (( i=1; i<=10; i++ ))
    do
        # get current system time as random seed
        seed=$(date +%s%3N)
        
        
        ./Main -g /data1/zhuoxh/timestamp/sorted_deduplicated_youtube-u-growth.txt -method I -resSize "$resSize" -seed "$seed"
        
        
        wait

        
        echo "Completed iteration $i with seed $seed and resSize $resSize"
    done
done

done
