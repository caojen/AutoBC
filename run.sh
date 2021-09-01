!/bin/bash

mkdir -p outputs

path="$(pwd)/Lasso-BC"
echo $path
filenames=$(ls $path)

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
    mkdir -p outputs/${filename}
    
    cd build
    
    if [ ! -f "/src/autobc/outputs/${filename}/rank_result.txt" ]; then
      echo "Running: ${filename}"
      ./ranking --ignore -8 /usr/bin/java -6 /jdk-16/bin/java -k 100 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux -f /src/autobc/outputs/${filename}/ref_result.txt -r /src/autobc/outputs/${filename}/random_result.txt | tee /src/autobc/outputs/${filename}/rank_result.txt
    fi
    sleep 1s
    if [ ! -f "/src/autobc/outputs/${filename}/rank_m_result.txt" ]; then
      echo "Running: ${filename}-m"
      ./ranking --ignore -8 /usr/bin/java -6 /jdk-16/bin/java -k 100 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux -f /src/autobc/outputs/${filename}/ref_result.txt -r /src/autobc/outputs/${filename}/random_result.txt --model | tee /src/autobc/outputs/${filename}/rank_m_result.txt
    fi
    sleep 1s
    cd -
  fi
done

