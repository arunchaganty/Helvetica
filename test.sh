#!/bin/bash

TESTS="tuples parser"

cd tests;
# Systematically test all programs
for t in $TESTS; do
    echo "Running test $t"
    ./$t
    if [[ $? == 0 ]]; then
        echo "Success..."
    else
        echo "Fail..."
    fi
done;

