#!/bin/sh
jsonpkg="json-c-master.tar.gz"
jsondir="json-c-master"
lib64dir="lib64"
libdir="lib"

#check if exist jsonc package
if [ ! -f "$jsonpkg" ]; then
	echo "Package $jsonpkg not found in current directory"
	exit
fi

#check is root
#if [ "$(id -u)" != "0" ]; then
#	echo "This script must be run as root"
#	exit
#fi

#install package
tar -xzvf $jsonpkg
cd $jsondir
./configure --prefix=$(pwd) --enable-shared=no --enable-static=yes --with-pic=true
make CFLAGS+=-fPIC
make install

if [ -d "$lib64dir" ]; then
	ln -s "$lib64dir" "lib"
elif [ -d "$libdir" ]; then
	ln -s "$libdir" "lib64"
else
	echo "Not exist lib or lib64 directory"	
	exit 1
fi

#setup env

echo ""
echo "-------------------------------------------------------------------------------"
echo " Json install finished. ready to set up JSON_ROOT environment to current path"
echo "export JSON_ROOT=`pwd`" >> ~/.bashrc
echo " JSON_ROOT environment set done"
source ~/.bashrc
