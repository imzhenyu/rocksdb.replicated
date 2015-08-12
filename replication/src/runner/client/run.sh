#!/bin/bash

rm -rf coredump/ meta/ replica* log.*
./rrdb config.ini -app client -app_index 1
