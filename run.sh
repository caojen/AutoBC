#!/bin/bash

path="$(pwd)/Lasso-BC"
echo $path
filenames=$(ls $path)

mkdir -p build/outputs
cd build
cmake .. && make
cd -

for filename in $filenames
do
  echo "Detecting files of test case: ${filename}"
  if [ ! -f "${path}/${filename}/${filename}.txt" ]; then
    echo "  No domain file. Ignored."
  elif [ ! -f "${path}/${filename}/generalBC.txt" ]; then
    echo "  No BC file. Ignored."
  else
    domainfile="${path}/${filename}/${filename}.txt"
    bcfile="${path}/${filename}/generalBC.txt"
    
    cd build
    echo "Running: ${filename}"
    ./autobc --global $domainfile --bcfile $bcfile --ignore -8 /usr/bin/java -6 /jdk-16/bin/java -k 100 -t 1000 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux | tee outputs/${filename}.tmp
    cd -
  fi
done
