#!/bin/bash

# mkdir -p outputs

# path="$(pwd)/Lasso-BC"
# echo $path
# filenames=$(ls $path)

# cd build
# cmake .. && make
# cd -

# for filename in $filenames
# do
#   echo "Detecting files of test case: ${filename}"
#   if [ ! -f "${path}/${filename}/${filename}.txt" ]; then
#     echo "  No domain file. Ignored."
#   elif [ ! -f "${path}/${filename}/generalBC.txt" ]; then
#     echo "  No BC file. Ignored."
#   else
#     domainfile="${path}/${filename}/${filename}.txt"
#     bcfile="${path}/${filename}/generalBC.txt"
#     mkdir -p outputs/${filename}
    
#     cd build
#     echo "Running: ${filename}"
#     ./ref_solver --global $domainfile --bcfile $bcfile --ignore -8 /usr/bin/java -6 /jdk-16/bin/java -k 1000 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux --output /src/autobc/outputs/${filename}/ref_result.txt
#     ./random_solver --global $domainfile --bcfile $bcfile --ignore -8 /usr/bin/java -6 /jdk-16/bin/java -k 1000 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux --output /src/autobc/outputs/${filename}/random_result.txt
#     # ./ranking --ignore -8 /usr/bin/java -6 /jdk-16/bin/java -k 1000 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux -f /src/autobc/outputs/${filename}/ref_result.txt -r /src/autobc/outputs/${filename}/random_result.txt | tee /src/autobc/outputs/${filename}/rank_result.txt
#     # ./ranking --ignore -8 /usr/bin/java -6 /jdk-16/bin/java -k 1000 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux -f /src/autobc/outputs/${filename}/ref_result.txt -r /src/autobc/outputs/${filename}/random_result.txt --model | tee /src/autobc/outputs/${filename}/rank_model_result.txt
#     cd -
#   fi
# done

mkdir -p ./outputs/telephone
cd build
./ref_solver --global /src/autobc/Lasso-BC/telephone/telephone.txt --bcfile /src/autobc/Lasso-BC/telephone/generalBC.txt --ignore -8 /usr/bin/java -6 /jdk-16/bin/java -k 100 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux --output /src/autobc/outputs/telephone/ref_result.txt
./random_solver --global /src/autobc/Lasso-BC/telephone/telephone.txt --bcfile /src/autobc/Lasso-BC/telephone/generalBC.txt --ignore -8 /usr/bin/java -6 /jdk-16/bin/java -k 100 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux --output /src/autobc/outputs/telephone/random_result.txt
cd -
