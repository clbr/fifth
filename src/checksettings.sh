#!/bin/bash

RED="$(echo -e '\033[1;31m')"
GREEN="$(echo -e '\033[1;32m')"
NORMAL="$(echo -e '\033[0;39m')"

before=`grep ST_ defaults.c`
after=`echo "$before" | sort`

beforemd5=`echo "$before" | md5sum`
aftermd5=`echo "$after" | md5sum`

if [ "$beforemd5" = "$aftermd5" ]; then
	echo -e "$GREEN"
	echo "All settings in order!"
	echo -e "$NORMAL"
else
	echo -e "$RED"
	echo -e "\n\tFail:\n\n"
	diff -u <(echo "$before") <(echo "$after")
	echo -e "$NORMAL"
fi
