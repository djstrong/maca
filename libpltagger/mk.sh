#!/bin/bash
cp boilerplate.h.in $1.h
U=`echo $1 | tr '[:lower:]' '[:upper:]'`
sed -i -s "s/\$UN/$U/" $1.h
cp boilerplate.cpp.in $1.cpp
sed -i -s "s/\$LN/$1/" $1.cpp
