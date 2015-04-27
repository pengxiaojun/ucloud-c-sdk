#!/bin/bash

doxytool="/usr/bin/doxygen"
echo "Ready to install jsonc"
cd 3rdparty
./json-make.sh
cd -
echo "jsonc install completed"

make -C src
make -C test
make -C tool

echo "Ready to generate sdk document"

if [ ! -f "$doxytool" ]; then
	echo "Not exist $doxytool tool"
	exit
fi

if [ ! -d "doc" ]; then
	mkdir doc
fi

cd include
$doxytool
cd -

echo "Document generated in doc dir."
echo "All done, Welcome to use ucloud-c-sdk ^_^^_^"
