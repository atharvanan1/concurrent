#! /bin/bash

set -e

cd `dirname $0`
rm -rf *.txt

for i in $(seq 14); do
    echo "Iteration ${i}"
    shuf -i0-2147483643 -n382 > case${i}.txt
    sort -n case${i}.txt > soln${i}.txt
    ./mysort case${i}.txt -o your${i}-merge.txt --alg=merge
    ./mysort case${i}.txt -o your${i}-quick.txt --alg=quick
    echo -n "Checking on Merge Sort - "
    cmp --silent your${i}-merge.txt soln${i}.txt && echo "Same" || echo "Different"
    echo -n "Checking on Quick Sort - "
    cmp --silent your${i}-quick.txt soln${i}.txt && echo "Same" || echo "Different"
done
