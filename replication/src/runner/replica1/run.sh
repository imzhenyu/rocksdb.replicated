#!/bin/bash

if [ "$1" == "true" ]
then
    echo "clear files"
    rm -rf coredump/ meta/ replica* log.* core.*
fi

echo "start server"
./rrdb config.ini -app replica -app_index 1 &>/dev/null &
