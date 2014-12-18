#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([Fifth],[0.2],[])
AC_CONFIG_AUX_DIR([config])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([-Wall foreign dist-bzip2 no-dist-gzip])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([src/autoconfig.h])
AC_PREFIX_DEFAULT([/usr])
AC_LANG_CPLUSPLUS


# Arguments
AC_ARG_ENABLE(distributable, AS_HELP_STRING([--enable-distributable],
				[Link all C++ and compiler libraries statically]),
				[distributable=$enableval],
				[distributable="no"])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_USE_SYSTEM_EXTENSIONS
dnl AM_GNU_GETTEXT
dnl AM_GNU_GETTEXT_VERSION([0.17])

AX_PTHREAD
LIBS=["$PTHREAD_LIBS $LIBS"]
CXXFLAGS=["$CXXFLAGS $PTHREAD_CFLAGS"]
CFLAGS=["$CFLAGS $PTHREAD_CFLAGS"]

# FLTK
unset fltkconfig
AC_CHECK_PROGS([fltkconfig], [fltk-config13 fltk-config])

if test "x$fltkconfig" = x; then
	AC_MSG_ERROR([fltk-config not found])
fi

VER=$($fltkconfig --version)
MINOR=$(echo $VER | cut -d. -f2)
PATCH=$(echo $VER | cut -d. -f3)

if test "$MINOR" -lt 3 || test "$PATCH" -lt 3; then
	AC_MSG_ERROR([FLTK >= 1.3.3 not found, got $VER])
fi

CXXFLAGS=["$CXXFLAGS $($fltkconfig --cxxflags | sed 's@-I@-isystem @g')"]
LIBS=["$LIBS $($fltkconfig --ldflags --use-images)"]

# Checks for libraries.
OLD_LDFLAGS=[$LDFLAGS]
LDFLAGS=["$LDFLAGS -lz"]
AC_CHECK_LIB([urlmatch], [url_init], [], AC_MSG_ERROR([liburlmatch not found]))
LDFLAGS=[$OLD_LDFLAGS]
AC_CHECK_LIB([dl], [dlopen], [], AC_MSG_ERROR([libdl not found]))
AC_CHECK_LIB([rt], [sched_get_priority_min], [], AC_MSG_ERROR([librt not found]))
AC_CHECK_LIB([z], [compress], [], AC_MSG_ERROR([libz not found]))
AC_CHECK_LIB([jpeg], [jpeg_set_quality], [], AC_MSG_ERROR([libjpeg not found]))
AC_CHECK_LIB([physfs], [PHYSFS_init], [], AC_MSG_ERROR([physfs not found]))
PKG_CHECK_MODULES([DEPS], [libxslt libxml-2.0 sqlite3 \
				freetype2 fontconfig cairo >= 1.12.18 libpng libcurl \
				openssl >= 0.9.8 glib-2.0])
# We don't require glib, but harfbuzz does, and doesn't list it in pkg-config.
if test "x$distributable" = "xyes"; then
	LIBS=["$LIBS -Wl,-Bstatic -static-libstdc++ -static-libgcc"]
fi
PKG_CHECK_MODULES([CPPDEPS], [harfbuzz harfbuzz-icu icu-uc icu-i18n webkitfltk])
LIBS=["$LIBS $CPPDEPS_LIBS"]
if test "x$distributable" = "xyes"; then
	LIBS=["$LIBS -Wl,-Bdynamic"]
fi

LIBS=["$LIBS $DEPS_LIBS"]
AC_CHECK_LIB([webkitfltk], [webkitInit], [], AC_MSG_ERROR([libwebkitfltk not found]))
CXXFLAGS=["$CXXFLAGS $DEPS_CFLAGS $CPPDEPS_CFLAGS"]

# Check for webkitfltk version
AC_MSG_CHECKING([webkitfltk version is ok])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <webkit.h>]],
	[[
	#if WK_FLTK_MAJOR != 0 || WK_FLTK_MINOR < 2
	#error unsupported webkitfltk version
	#endif
	]])], [AC_MSG_RESULT([yes])], [AC_MSG_FAILURE([Unsupported webkit version, 0.2 required])]
	)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Random other things
CFLAGS=["$CFLAGS -Wall -Wextra"]
CXXFLAGS=["$CXXFLAGS -Wall -Wextra -std=gnu++11"]
CXXFLAGS=["$CXXFLAGS -ffunction-sections -fdata-sections"]
LDFLAGS=["$LDFLAGS -Wl,-O1 -Wl,-rpath,'\$\$ORIGIN'"]

AC_CONFIG_COMMANDS([atag],[
echo -e "\n\n\t\t*********\n"
echo Configure finished
echo
echo Distributable binary: $distributable
echo -e "\n\t\t*********\n\n"],
[distributable=$distributable])

AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile])
AC_OUTPUT
