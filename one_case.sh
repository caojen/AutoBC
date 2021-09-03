#!/bin/bash
casename=$1
echo "casename: $casename"
domainfile=/src/autobc/Lasso-BC/${casename}/${casename}.txt
echo "domainfile: $domainfile"
workspace=/src/autobc/output1.3/${casename}
echo "workspace: $workspace"
ls $workspace
files=$(ls ${workspace})

for cont in $files
do
  bcfile=$workspace/$cont
  echo "bcfile: $bcfile"
  target_dir=/src/autobc/experiment1.3/${casename}/${cont}
  echo "target_dir: $target_dir"
  mkdir -p $target_dir

  nohup ./one_case_cont.sh $domainfile $bcfile $target_dir > /src/autobc/experiment1.3.output/${casename}.${cont}.output &
  sleep 1s
done
