#l!/bin/bash
#rm -rf ./src/output
#mkdir ./src/ouput
#echo 'sheduler is: ' $scheduler

for ((i = 1; i < 10; i++)); do
   ./obj/station $i
    #cp sim.data ./output/sim-1-$i-001.data
done
