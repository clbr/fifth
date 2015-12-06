#!/bin/sh

ver=unknown
enab=0

[ -n "`which git`" ] && [ -d ../.git ] && ver=`git describe` && enab=1

cat > version.h << EOF
#ifndef VER_H
#define VER_H

#define GITVERSION "$ver"
#define GIT $enab

#endif
EOF
