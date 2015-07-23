#!/bin/bash
# Run `make dbg' first to generate test binaries.

function run()
{
    echo "===========" $1 "==========="

    if [ "$1" == "./db_sanity_test" ]
    then
        ./$1 "${1}.dir" create
        ./$1 "${1}.dir" verify
    else
        ./$1
    fi

    if [ $? -ne 0 ]; then
        echo "TEST FAILED!!!"
        exit 1
    fi
}

num=0
for test_case in `find . -type f -name 'test_*' -perm -100`
do
    run $test_case
    num=$((num+1))
done
for test_case in `find . -type f -name '*_test' -perm -100`
do
    run $test_case
    num=$((num+1))
done

echo
echo "CASE NUM: $num"
echo "ALL CASE PASSED!!!"
