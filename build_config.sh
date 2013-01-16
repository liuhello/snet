#!/bin/bash

OUTPUT=$1
PREFIX=$2

if test -z "$OUTPUT" || test -z "$PREFIX";
then
    echo "usage: $0 <output-filename> <directory-prefix>"
    exit 1
fi

rm -rf $OUTPUT
touch $OUTPUT

DIRS="$PREFIX/snet"

FILES=`find $DIRS -name '*.cc' -print | sort | sed "s,^$PREFIX/,," | tr "\n" " "`

echo "SOURCES=$FILES" >> $OUTPUT
