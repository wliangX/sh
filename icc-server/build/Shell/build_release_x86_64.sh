#! /bin/bash

CURRENT_DIR=$(cd $(dirname $0); pwd)
ICC_ROOT=$(cd $(dirname $0);cd ../..; pwd)

build(){
	echo "...ICC_ROOT: $ICC_ROOT"
	
	cd $ICC_ROOT/ThirdParty/Linux/Release/x86/
	echo "...tar -zxf Linux.tar.gz"
	tar -zxf Linux.tar.gz
	cd $ICC_ROOT
	
	cmake –S ./ -B ./Build/CMake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CROSS_COMPILE=0 -DPLATFORM=x86
	cd $ICC_ROOT/Build/CMake/
	core_num=$(nproc)
	echo "...core_num:"$core_num
	
	if [ $core_num -gt 4 ]; then
		core_num=4
	else
		core_num=2
	fi	
	
	echo "...make_core_num:"$core_num
	make -j $core_num
	
	if [ $? -ne 0 ];then
		echo " make  -- Faile  : "$?
		exit 1
	else
		echo " make  -- Success !"
	fi

	cd $ICC_ROOT/Build/Shell/
	source ./after_build.sh Release Linux x86
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


