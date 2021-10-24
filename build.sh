#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Usage ./build.sh <main cpp filename>"
fi

filename=${1}
exe_name="${filename%.*}"

cmd="g++ -g -O2 -std=c++11 $filename utils.cpp -lm -lpng -o $exe_name"
echo $cmd
eval $cmd
