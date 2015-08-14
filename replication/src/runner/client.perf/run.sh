#!/bin/bash

rm -rf coredump/ meta/ replica* log.* core.*
./rrdb config.ini -app client.perf -app_index 1
