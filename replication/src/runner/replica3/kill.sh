#!/bin/sh

netstat -nap | grep LISTEN | grep 34803 | awk '{print $7}' | cut -d'/' -f1 | xargs kill

