#!/bin/bash

rm -f results.txt
cp test_data/* .

echo -n $1 > results.txt
echo -n ", " >> results.txt

./test_ex1
./test_ex2

rm -f __TEST_FILE__

