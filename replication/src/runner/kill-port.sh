#!/bin/sh

if [ $# -ne 1 ]
then
    echo "USAGE: $0 <port>"
    exit
fi

netstat -nap | grep LISTEN | grep ":$1 "
netstat -nap | grep LISTEN | grep ":$1 " | awk '{print $7}' | cut -d'/' -f1 | xargs kill

