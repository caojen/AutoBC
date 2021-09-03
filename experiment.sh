#!/bin/bash

mkdir -p experiment1.3
outputdir="$(pwd)/output1.3"

cases=$(ls ${outputdir})
ref_bin="$(pwd)/build/ref_solver"
random_bin="$(pwd)/build/random_solver"
ranking_bin="$(pwd)/build/ranking"

cd build && cmake .. && make && cd -

for one_case in $cases
do
  nohup $(pwd)/one_experiment.sh $one_case > ${one_case}.output &
done
