#!/bin/bash
BUILDDIR=qtcreator-build
PROJECTS="corpus2 toki maca"
BASEDIR=../..
CURDIR=`pwd`
FILTER="grep -i -C 4 error"
#FILTER=cat

function fail() {
	echo "!!! $1 in $i !!!"
	cd $CURDIR
	exit 1
}

for i in $PROJECTS; do
	echo
	echo "--- Building $i..."
	cd $BASEDIR/$i/$BUILDDIR || fail  "cd error"
	cmake .. > /dev/null || fail "cmake error"
	make -j4 > /dev/null || fail  "make error"
	echo "--- Installing $i..."
	sudo make install > /dev/null || fail "make install error"
	echo "--- Testing $i..."
	make test > /dev/null || fail "make test error in"
done
cd $CURDIR
sudo ldconfig

