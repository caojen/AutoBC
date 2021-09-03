#!/bin/bash
domainfile=$1
bcfile=$2
target_dir=$3

mkdir -p $target_dir

echo $domainfile $bcfile $target_dir

ref_bin="$(pwd)/build/ref_solver"
random_bin="$(pwd)/build/random_solver"
ranking_bin="$(pwd)/build/ranking"

ref_result_file="${target_dir}/ref_result.txt"

if [ ! -f ${ref_result_file} ];then
  ${ref_bin} --global ${domainfile} --bcfile ${bcfile} -8 /usr/bin/java -6 /jdk-16/bin/java -k 100 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar --nuxmv /src/autobc/bin/nuXmv-2.0.0-Linux --output ${ref_result_file}
  sleep 1s
fi

random_result_file="${workspace}/random_result.txt"

if [ ! -f ${random_result_file} ];then
  ${random_bin} --global ${domainfile} --bcfile ${bcfile} -8 /usr/bin/java -6 /jdk-16/bin/java -k 100 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar --nuxmv /src/autobc/bin/nuXmv-2.0.0-Linux --output ${random_result_file}
  sleep 1s
fi

# 排序，生成普通文件 rank_result
  
rank_result_file="${workspace}/rank_result.txt"
if [ ! -f ${rank_result_file} ];then
  ${ranking_bin} -8 /usr/bin/java -6 /jdk-16/bin/java -k 100 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux -f ${ref_result_file} -r ${random_result_file} | tee ${rank_result_file}
  sleep 1s
fi

# 排序，生成普通文件 rank_m_result
rank_m_result_file="${workspace}/rank_m_result.txt"
if [ ! -f ${rank_m_result_file} ];then
  ${ranking_bin} -8 /usr/bin/java -6 /jdk-16/bin/java -k 100 -t 100 --likelyhood /src/autobc/Lasso-BC/likelyhood.jar --modelcounting /src/autobc/jar/modelcounting.jar -n /src/autobc/bin/nuXmv-2.0.0-Linux -f ${ref_result_file} -r ${random_result_file} --model | tee ${rank_m_result_file}
  sleep 1s
fi
