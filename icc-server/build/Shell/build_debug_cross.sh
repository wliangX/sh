#! /bin/bash

CURRENT_DIR=$(cd $(dirname $0); pwd)
ICC_ROOT=$(cd $(dirname $0);cd ../..; pwd)
CROSS_COMPILE_PATH=$(find / -type f -name 'aarch64-linux-gnu-gcc')
CROSS_COMPILE_PATH=${CROSS_COMPILE_PATH%/*}

if [ ! -n $CROSS_COMPILE_PATH ]; then
	echo "Fail:  cross compiler cannot find !"
	exit 1
fi

build(){
	echo "...ICC_ROOT: $ICC_ROOT"
	
	cd $ICC_ROOT/ThirdParty/Linux/Debug/arm/
	tar -zxf Linux_arm.tar.gz
	cd $ICC_ROOT
	
	cmake –S ./ -B ./Build/CMake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CROSS_COMPILE=1 -DCROSS_COMPILE_PATH=$CROSS_COMPILE_PATH -DPLATFORM=arm
	cd $ICC_ROOT/Build/CMake/
	make
	cd $ICC_ROOT/Build/Shell/
	source ./after_build.sh Debug Linux arm
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


