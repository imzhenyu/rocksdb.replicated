#!/bin/sh

netstat -nap | grep LISTEN | grep 34601 | awk '{print $7}' | cut -d'/' -f1 | xargs kill

