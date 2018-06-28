#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=. 

THEDIR=`pwd`
cd $srcdir
DIE=0

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have autoconf installed to compile libxslt."
	echo "Download the appropriate package for your distribution,"
	echo "or see http://www.gnu.org/software/autoconf"
	DIE=1
}

(libtool --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have libtool installed to compile libxslt."
	echo "Download the appropriate package for your distribution,"
	echo "or see http://www.gnu.org/software/libtool"
	DIE=1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
	echo
	DIE=1
	echo "You must have automake installed to compile libxslt."
	echo "Download the appropriate package for your distribution,"
	echo "or see http://www.gnu.org/software/automake"
}

if test "$DIE" -eq 1; then
	exit 1
fi

test -f libxslt/xslt.h  || {
	echo "You must run this script in the top-level libxslt directory"
	exit 1
}

if test -z "$*"; then
	echo "I am going to run ./configure with no arguments - if you wish "
        echo "to pass any to it, please specify them on the $0 command line."
fi

echo "Running libtoolize..."
libtoolize --copy --force
echo "Running aclocal..."
aclocal $ACLOCAL_FLAGS
echo "Running automake..."
automake --add-missing
echo "Running autoconf..."
autoconf

cd $THEDIR

if test x$OBJ_DIR != x; then
    mkdir -p "$OBJ_DIR"
    cd "$OBJ_DIR"
fi

echo Running configure "$@" ...
$srcdir/configure "$@"

echo 
echo "Now type 'make' to compile libxslt."
