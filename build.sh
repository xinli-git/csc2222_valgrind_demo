#!/bin/bash

exe_name=$1

g++ -g -O0 -std=c++11 main.cpp -lm -lpng -o $exe_name
