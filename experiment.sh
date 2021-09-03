#!/bin/bash

mkdir -p experiment1.3
mkdir -p experiment1.3.output
outputdir="$(pwd)/output1.3"

cases=$(ls ${outputdir})
ref_bin="$(pwd)/build/ref_solver"
random_bin="$(pwd)/build/random_solver"
ranking_bin="$(pwd)/build/ranking"

cd build && cmake .. && make && cd -

for one_case in $cases
do
  nohup $(pwd)/one_experiment.sh $one_case > experiment1.3.output/${one_case}.output &
  sleep 1s
done
