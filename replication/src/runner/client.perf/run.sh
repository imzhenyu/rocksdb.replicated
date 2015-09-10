#!/bin/bash

APP=client.perf
APP_INDEX=1

if [ $# != 2 ]
then
    echo "USAGE: $0 <config-file> <clear-flag>"
    echo "  e.g. $0 config.ini true"
    exit 1
fi

if [ ! -f "$1" ]
then
    echo "ERROR: config file not exist"
    exit 1
fi

if [ "$2" != "true" -a "$2" != "false" ]
then
    echo "ERROR: clear flag should be true or false"
    exit 1
fi

if [ "$2" == "true" ]
then
    echo "clear files"
    rm -rf coredump/ meta/ replica* log.* core.* result
fi

echo "start client"
./rrdb $1 -app $APP -app_index $APP_INDEX

