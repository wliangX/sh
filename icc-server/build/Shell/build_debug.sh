#! /bin/bash

CURRENT_DIR=$(cd $(dirname $0); pwd)
ICC_ROOT=$(cd $(dirname $0);cd ../..; pwd)

build(){
	echo "...ICC_ROOT: $ICC_ROOT"
	
	cd $ICC_ROOT/ThirdParty/Linux/Debug/x86/
	tar -zxf Linux.tar.gz
	cd $ICC_ROOT
	
	cmake –S ./ -B ./Build/CMake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CROSS_COMPILE=0 -DPLATFORM=x86
	cd $ICC_ROOT/Build/CMake/
	make
	cd $ICC_ROOT/Build/Shell/
	source ./after_build.sh Debug Linux x86
}

clean(){
	cd $ICC_ROOT/Build/CMake/
	make clean
	rm -rf $ICC_ROOT/Build/CMake/
}

case $1 in
clean):
    clean
    ;;	
*):
	echo "...................."
	build
    ;;
esac
	
exit 0


