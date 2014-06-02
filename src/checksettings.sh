#!/bin/bash

before=`grep ST_ defaults.c`
after=`echo "$before" | sort`

beforemd5=`echo "$before" | md5sum`
aftermd5=`echo "$after" | md5sum`

if [ "$beforemd5" = "$aftermd5" ]; then
	echo "All settings in order!"
else
	echo -e "\n\tFail:\n\n"
	diff -u <(echo "$before") <(echo "$after")
fi
