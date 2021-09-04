#!/bin/bash

ref_bin="$(pwd)/build/ref_solver"
outputdir="$(pwd)/output1.3"
cases=$(ls ${outputdir})

for one_case in $cases
do
  casepath="$(pwd)/output1.3/${one_case}"
  domainfile="$(pwd)/Lasso-BC/${one_case}/${one_case}.txt"
  bcfile="${casepath}/contrasty11.txt"
  echo $domainfile $bcfile
  ${ref_bin} --global ${domainfile} --bcfile ${bcfile} -8 /usr/bin/java -6 /jdk-16/bin/java -k 100 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar --nuxmv /src/autobc/bin/nuXmv-2.0.0-Linux --output $(pwd)/sort_outputs/output1.3.${one_case}.txt > $(pwd)/sort_outputs/output1.3.${one_case}.output || echo error
  sleep 1s
done
