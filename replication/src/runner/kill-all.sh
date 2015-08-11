#!/bin/bash

ps -ef | grep rrdb | awk '{print $2}' | xargs kill
