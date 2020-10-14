#! /bin/bash

for i in seq 0 5; do
	./mysort input.txt -o merge-output.txt --alg=lkbucket | grep "\- $i" | wc -l
done
