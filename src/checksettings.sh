#!/bin/bash

RED="$(echo -e '\033[1;31m')"
GREEN="$(echo -e '\033[1;32m')"
NORMAL="$(echo -e '\033[0;39m')"

before=`grep ST_ defaults.c | cut -d, -f1`
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

# Now check types
lines=`grep ST_ defaults.c | cut -dS -f2- | cut -d_ -f2- | cut -d= -f1 | sed 's@, { .@ @g'`

IFS="
"

for i in `echo "$lines"`; do
	def=`echo $i | cut -d " " -f1 | tr [[:upper:]] [[:lower:]]`
	letter=`echo $i | cut -d " " -f2`

	def=${def:0:1}

	[ "$def" = "$letter" ] || echo -e "${RED}Failed type check $i $NORMAL"
done
