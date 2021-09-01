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
#   echo "Detecting files of test case: achievepattern"
#   if [ ! -f "${path}/achievepattern/achievepattern.txt" ]; then
#     echo "  No domain file. Ignored."
#   elif [ ! -f "${path}/achievepattern/generalBC.txt" ]; then
#     echo "  No BC file. Ignored."
#   else
#     domainfile="${path}/achievepattern/achievepattern.txt"
#     bcfile="${path}/achievepattern/generalBC.txt"
#     mkdir -p outputs/achievepattern
    
#     cd build
#     echo "Running: achievepattern"
#     # ./ref_solver --global $domainfile --bcfile $bcfile --ignore -8 /usr/bin/java -6 /jdk-16/bin/java -k 1000 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux --output /src/autobc/outputs/achievepattern/ref_result.txt
#     # ./random_solver --global $domainfile --bcfile $bcfile --ignore -8 /usr/bin/java -6 /jdk-16/bin/java -k 1000 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux --output /src/autobc/outputs/achievepattern/random_result.txt
#     # ./ranking --ignore -8 /usr/bin/java -6 /jdk-16/bin/java -k 1000 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux -f /src/autobc/outputs/achievepattern/ref_result.txt -r /src/autobc/outputs/achievepattern/random_result.txt | tee /src/autobc/outputs/achievepattern/rank_result.txt
#     # ./ranking --ignore -8 /usr/bin/java -6 /jdk-16/bin/java -k 1000 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux -f /src/autobc/outputs/achievepattern/ref_result.txt -r /src/autobc/outputs/achievepattern/random_result.txt --model | tee /src/autobc/outputs/achievepattern/rank_m_result.txt
#     sleep 1s
#     cd -
#   fi
# done

cd build
./ranking --ignore -8 /usr/bin/java -6 /jdk-16/bin/java -k 100 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux -f /src/autobc/outputs/liftcontroller/ref_result.txt -r /src/autobc/outputs/liftcontroller/random_result.txt | tee /src/autobc/outputs/liftcontroller/rank_result.txt
./ranking --ignore -8 /usr/bin/java -6 /jdk-16/bin/java -k 100 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux -f /src/autobc/outputs/liftcontroller/ref_result.txt -r /src/autobc/outputs/liftcontroller/random_result.txt --model | tee /src/autobc/outputs/liftcontroller/rank_m_result.txt
cd -
