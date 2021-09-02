#!/bin/bash

mkdir -p experiment1.3
outputdir="$(pwd)/output1.3"

cases=$(ls ${outputdir})

for one_case in $cases
do
  echo "Case: ${one_case}"
  casepath="$(pwd)/output1.3/${one_case}"
  domainfile="$(pwd)/Lasso-BC/${one_case}/${one_case}.txt"
  echo "- Domain&Goal File: ${domainfile}"

  contrasties=$(ls ${casepath})
  for contrasty in $contrasties:
  do
    echo "- - running: ${one_case}: ${contrasty}"
    bcfile="${casepath}/${contrasty}.txt"
    echo "- - BC File: ${bcfile}"
  done
done
