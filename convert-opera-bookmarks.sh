#!/bin/sh -e

if [ "$#" -ne 2 ]; then
	echo "This script converts bookmarks from Opera's adr format to that used by Fifth."
	echo "Usage: $0 input output"
	echo
	[ -f ~/.opera/opera6.adr ] && echo "Found opera6.adr at ~/.opera/opera6.adr"
	[ -f ~/.opera/bookmarks.adr ] && echo "Found bookmarks.adr at ~/.opera/bookmarks.adr"
	echo
	exit 0
fi

src=$1
dst=$2

IFS="
"
tab="	"

> "$dst"
type=0

awk 'BEGIN { RS="\n\n"; FS="\n\t" }
	{ name=""; url=""; for (i = 2; i < NF; i++) {
		if ($i ~ /^NAME/) {
			name = $i
			sub(/^NAME=/, "", name)
			gsub(/\//, "\\\\", name)
			# FLTK does not like fwd slashes in names
		} else if ($i ~ /^URL/) {
			url = $i
			sub(/^URL=/, "", url)
		}
	}

	if ($1 == "#FOLDER") {
		print "name " name
		print "url "
		print ""
	} else if ($1 == "#URL") {
		print "name " name
		print "url " url
		print ""
	} else if ($1 == "-") {
		print "name "
		print "url "
		print ""
	}

	}' < "$src" > "$dst"
