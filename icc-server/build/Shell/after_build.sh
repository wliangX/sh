#!/bin/bash
if [ -z "$1" ]; then
	echo "Fail:  You must specify two parameters!"
	echo "Example:  ./after_build.sh Debug[Release] Linux"
	exit 1
fi

#****************************************************************************
#init path
#****************************************************************************
ICC_ROOT=$(cd $(dirname $0);cd ../..; pwd)
BUILD_TYPE=$1
HOST_SYSTEM=$2
PLATFORM=$3

if [ ! -n $BUILD_TYPE ]; then
	echo "...BUILD_TYPE is empty, exit"
	exit
fi

if [ ! -n $HOST_SYSTEM ]; then
	echo "...HOST_SYSTEM is empty, exit"
	exit
fi

if [ ! -n $PLATFORM ]; then
	echo "...PLATFORM is empty, exit"
	exit
fi

echo "...PLATFORM: $PLATFORM"
if [ "$PLATFORM" = "arm" ];then
	CROSS_COMPILE_PATH=$(find /home/software/ -type f -name 'aarch64-linux-gnu-gcc')
	CROSS_COMPILE_PATH=${CROSS_COMPILE_PATH%/*}
	echo "...CROSS_COMPILE_PATH: $CROSS_COMPILE_PATH"
fi

BUILD_PATH=$ICC_ROOT/Build/$BUILD_TYPE
PACKAGE_PATH=$ICC_ROOT/Build/Package
CONF_PATH=$PACKAGE_PATH/$BUILD_TYPE/icc
TOOS_PATH=$ICC_ROOT/Tools
THIRD_PATH=$ICC_ROOT/ThirdParty/$HOST_SYSTEM/$BUILD_TYPE/$PLATFORM

rm -rf "$PACKAGE_PATH"
mkdir -p "$PACKAGE_PATH"

rm -rf "$CONF_PATH"
mkdir -p "$CONF_PATH"

#****************************************************************************
# output information
#****************************************************************************
echo "...BUILD_PATH: $BUILD_PATH"
echo "...PACKAGE_PATH: $PACKAGE_PATH"
echo "...TOOS_PATH: $TOOS_PATH"
echo "...THIRD_PATH: $THIRD_PATH"

#****************************************************************************
# copy package
#****************************************************************************
if [ ! -f BuildProcessList.txt ]; then
	echo "BuildProcessList is not a file, exit"
	exit
fi

echo "...Begin copy files，waiting please！"
for fc in `cat BuildProcessList.txt`; do
	array=(`echo $fc | tr '.' ' '`)
	
	if [ ${#array[@]} -ne 4 ]; then
		continue
	fi		
		
	TYPE=${array[2]}
	SUBTYPE=${array[3]}
	if [ "$SUBTYPE" != "CTI" ]; then
		SRV_NAME=${array[0]}.${array[1]}.${array[2]}
		PKG_SRV_PATH=$PACKAGE_PATH/$BUILD_TYPE/$TYPE
	else
		SRV_NAME=${array[0]}.${array[1]}.${array[3]}
		PKG_SRV_PATH=$PACKAGE_PATH/$BUILD_TYPE/$SUBTYPE
	fi
	echo "++++++++++++++++++TYPE: $TYPE"
	echo "++++++++++++++++++SUBTYPE: $SUBTYPE"
	echo "++++++++++++++++++SRV_NAME: $SRV_NAME"
	echo "++++++++++++++++++PKG_SRV_PATH: $PKG_SRV_PATH"
	
	mkdir -p "$PKG_SRV_PATH/Manager/"
	mkdir -p "$PKG_SRV_PATH/Components/"
	mkdir -p "$PKG_SRV_PATH/Plugins/"	
	
	#copy depend files	
	find "$THIRD_PATH/activemq/lib/" -type f -name "libactivemq-cpp.so*" -exec cp -rf {} 	"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/activemq/lib/" -type l -name "libactivemq-cpp.so*" -exec cp -rf {} 	"$PKG_SRV_PATH/" ";"	
	find "$THIRD_PATH/activemq/lib/" -type f -name "libapr-1.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/activemq/lib/" -type l -name "libapr-1.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	
	find "$THIRD_PATH/redis/lib/"	 -type f -name "lib*.so" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"	
	find "$THIRD_PATH/postgres/lib/" -type f -name "libpq.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/postgres/lib/" -type l -name "libpq.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	
	find "$THIRD_PATH/oracle/lib/" -type f -name "libaio.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/oracle/lib/" -type l -name "libaio.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/oracle/lib/" -type f -name "libocci.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/oracle/lib/" -type l -name "libocci.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/oracle/lib/" -type f -name "libociei.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/oracle/lib/" -type l -name "libociei.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/oracle/lib/" -type f -name "libclntsh.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/oracle/lib/" -type l -name "libclntsh.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/oracle/lib/" -type f -name "libclntshcore.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/oracle/lib/" -type l -name "libclntshcore.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	
	if [ "$PLATFORM" = "arm" ];then
		find "$THIRD_PATH/oracle/lib/" -type f -name "libnnz19.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
		find "$THIRD_PATH/oracle/lib/" -type l -name "libnnz19.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	else
		find "$THIRD_PATH/oracle/lib/" -type f -name "libnnz21.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
		find "$THIRD_PATH/oracle/lib/" -type l -name "libnnz21.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	fi
	
	find "$THIRD_PATH/openssl/lib/" -type f -name "libssl.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/openssl/lib/" -type l -name "libssl.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	
	find "$THIRD_PATH/openssl/lib/" -type f -name "libcrypto.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/openssl/lib/" -type l -name "libcrypto.so*" -exec cp -rf {} 			"$PKG_SRV_PATH/" ";"
	
	find "$THIRD_PATH/boost/stage/lib/" -type f -name "libboost*.so*" -exec cp -rf {} 	"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/boost/stage/lib/" -type l -name "libboost*.so*" -exec cp -rf {} 	"$PKG_SRV_PATH/" ";"

	find "$THIRD_PATH/libpcap/lib/" -type f -name "libpcap.so*" -exec cp -rf {} 	"$PKG_SRV_PATH/" ";"
	find "$THIRD_PATH/libpcap/lib/" -type l -name "libpcap.so*" -exec cp -rf {} 	"$PKG_SRV_PATH/" ";"
	
	if [ "$PLATFORM" != "arm" ];then
		find "$THIRD_PATH/nacos/lib/" -type f -name "lib*.so*" -exec cp -rf {} 	"$PKG_SRV_PATH/" ";"
		find "$THIRD_PATH/nacos/lib/" -type l -name "lib*.so*" -exec cp -rf {} 	"$PKG_SRV_PATH/" ";"
	else
		find "$THIRD_PATH/nacos/lib/" -type f -name "libnacos-cli.so*" -exec cp -rf {} 	"$PKG_SRV_PATH/" ";"
	fi	
	
	if [ "$PLATFORM" = "x86" ];then
		find "/usr/lib" -type f -name "libstdc++.so.6*" -exec cp -rf {} 	"$PKG_SRV_PATH/" ";"
		find "/usr/lib" -type l -name "libstdc++.so.6*" -exec cp -rf {} 	"$PKG_SRV_PATH/" ";"
	elif [ "$PLATFORM" = "x64" ];then
		find "/usr/lib64" -type f -name "libstdc++.so.6*" -exec cp -rf {} 	"$PKG_SRV_PATH/" ";"
		find "/usr/lib64" -type l -name "libstdc++.so.6*" -exec cp -rf {} 	"$PKG_SRV_PATH/" ";"
	elif [ "$PLATFORM" = "arm" ];then
		find "$CROSS_COMPILE_PATH/../aarch64-linux-gnu/lib64/" -type f -name "libstdc++.so*" -exec cp -rf {} 	"$PKG_SRV_PATH/" ";"
		find "$CROSS_COMPILE_PATH/../aarch64-linux-gnu/lib64/" -type l -name "libstdc++.so*" -exec cp -rf {} 	"$PKG_SRV_PATH/" ";"
	else
		echo "Fail: platform parameter error!"
		exit 1
	fi
		
	#copy build files
	cp -rf "$BUILD_PATH/Console"		 											"$PKG_SRV_PATH/$SRV_NAME"
	cp -rf "$BUILD_PATH/libFrame.so" 												"$PKG_SRV_PATH/"
	cp -rf "$BUILD_PATH/Plugins/Public/libMessage.so"								"$PKG_SRV_PATH/Plugins/"
	find "$BUILD_PATH/Manager/" -type f -name "*.so" -exec cp {} 					"$PKG_SRV_PATH/Manager/" ";"
	find "$BUILD_PATH/Components/" -type f -name "*.so" -exec cp {} 				"$PKG_SRV_PATH/Components/" ";"	
	
	if [ "$SUBTYPE" != "CTI" ];then
		find "$BUILD_PATH/Plugins/$TYPE/$SUBTYPE" -type f -name "*.so" -exec cp {} 				"$PKG_SRV_PATH/Plugins/" ";"
	else
		cp -rf "$BUILD_PATH/Plugins/$TYPE/$SUBTYPE/libCTI.so"  							"$PKG_SRV_PATH/Plugins/" 
		cp -rf "$BUILD_PATH/Plugins/$TYPE/$SUBTYPE/libDBProcess.so"  					"$PKG_SRV_PATH/Plugins/"
		cp -rf "$BUILD_PATH/Plugins/$TYPE/$SUBTYPE/libTSAPIClient.so" 	 				"$PKG_SRV_PATH/Components/"
		cp -rf "$BUILD_PATH/Plugins/$TYPE/$SUBTYPE/libFSAESClient.so" 	 				"$PKG_SRV_PATH/Components/"
	fi
		
	#build config.config
	echo [ICC]>$PKG_SRV_PATH/Config.Config
	if [ "$SUBTYPE" != "CTI" ]; then
		echo ConfigPath=../Tools/ConfigFiles/$TYPE/Config.xml>>$PKG_SRV_PATH/Config.Config
		echo ConfigFileDir=../Tools/ConfigFiles/$TYPE/>>$PKG_SRV_PATH/Config.Config
		echo SqlConfigPath=../Tools/ConfigFiles/Sql/sql.xml>>$PKG_SRV_PATH/Config.Config
		echo StaticConfigPath=../Tools/ConfigFiles/$TYPE/StaticConfig.xml>>$PKG_SRV_PATH/Config.Config
	else
		echo ConfigPath=../Tools/ConfigFiles/$SUBTYPE/Config.xml>>$PKG_SRV_PATH/Config.Config
		echo ConfigFileDir=../Tools/ConfigFiles/$SUBTYPE/>>$PKG_SRV_PATH/Config.Config
		echo SqlConfigPath=../Tools/ConfigFiles/Sql/sql.xml>>$PKG_SRV_PATH/Config.Config
		echo StaticConfigPath=../Tools/ConfigFiles/$SUBTYPE/StaticConfig.xml>>$PKG_SRV_PATH/Config.Config
	fi
	
	#copy run.sh
	cp -rf "$ICC_ROOT/Build/Shell/run.sh" "$PKG_SRV_PATH/"
	chmod 744 $PKG_SRV_PATH/run.sh
	
	cp -rf "$ICC_ROOT/Build/Shell/docker-entrypoint.sh" "$PKG_SRV_PATH/"
	chmod 744 $PKG_SRV_PATH/docker-entrypoint.sh
done


#copy Tools
cp -rf "$TOOS_PATH/" "$PACKAGE_PATH/$BUILD_TYPE/"
echo "...End copy files"

#Compress Packages
commpress_pkg(){
	SERVER_TYPE=$1
	echo "...Compress: $SERVER_TYPE"
	if [ "$SERVER_TYPE" = "" ]; then 
		echo "Unknowe server name: $SERVER_TYPE"
		exit 1
	fi
	echo $PACKAGE_PATH
	echo $BUILD_TYPE
	echo $SERVER_TYPE
	cd $PACKAGE_PATH/$BUILD_TYPE/
	mkdir -p CommandCenter.ICC.$SERVER_TYPE/

	cp -rf $SERVER_TYPE/ CommandCenter.ICC.$SERVER_TYPE/
	mv ./CommandCenter.ICC.$SERVER_TYPE/$SERVER_TYPE ./CommandCenter.ICC.$SERVER_TYPE/CommandCenter.ICC.$SERVER_TYPE

	#mkdir -p CommandCenter.ICC.$SERVER_TYPE/ConfigFiles/ManualSql/
	#mkdir -p CommandCenter.ICC.$SERVER_TYPE/ConfigFiles/$SERVER_TYPE/
	#mkdir -p CommandCenter.ICC.$SERVER_TYPE/ConfigFiles/SDG/$SERVER_TYPE/
	mkdir -p $CONF_PATH/$SERVER_TYPE/conf/
	mkdir -p CommandCenter.ICC.$SERVER_TYPE/Tools/ConfigFiles/bak/

	cp -rf ../../../Tools/ CommandCenter.ICC.$SERVER_TYPE/
	#cp -f Tools/ConfigFiles/$SERVER_TYPE/Config.xml CommandCenter.ICC.$SERVER_TYPE/ConfigFiles/$SERVER_TYPE/
	#cp -f Tools/ConfigFiles/$SERVER_TYPE/StaticConfig.xml CommandCenter.ICC.$SERVER_TYPE/ConfigFiles/$SERVER_TYPE/
	#cp -f Tools/ConfigFiles/SDG/$SERVER_TYPE/appsettings.json CommandCenter.ICC.$SERVER_TYPE/ConfigFiles/SDG/$SERVER_TYPE/
	cp -r Tools/ConfigFiles/$SERVER_TYPE/docker-compose.yml CommandCenter.ICC.$SERVER_TYPE/
	cp -f Tools/ConfigFiles/$SERVER_TYPE/Config.xml $CONF_PATH/$SERVER_TYPE/conf/
	cp -f Tools/ConfigFiles/$SERVER_TYPE/StaticConfig.xml $CONF_PATH/$SERVER_TYPE/conf/
	cp -rf Tools/ConfigFiles/$SERVER_TYPE/ CommandCenter.ICC.$SERVER_TYPE/Tools/ConfigFiles/bak/

	if [ "$SERVER_TYPE" = "Server" ]; then 
		mkdir -p $CONF_PATH/$SERVER_TYPE/conf/ManualSql/
		cp -f Tools/DataBaseScript/init_Database.sql $CONF_PATH/$SERVER_TYPE/conf/
		cp -f Tools/DataBaseScript/init_MassData.sql $CONF_PATH/$SERVER_TYPE/conf/ManualSql/
	fi
	if [ "$PLATFORM" = "arm" ];then
		cp -f ../../Dockerfile/$SERVER_TYPE/Dockerfile.arm64 CommandCenter.ICC.$SERVER_TYPE/
	else	
        cp -f ../../Dockerfile/$SERVER_TYPE/Dockerfile.x86_64 CommandCenter.ICC.$SERVER_TYPE/
	fi	
	#cp -f ../../Shell/docker.sh CommandCenter.ICC.$SERVER_TYPE/CommandCenter.ICC.$SERVER_TYPE.Install.sh

	# 构建软件包时把配置文件拷贝到 Package 文件 中
	#if [ "$SERVER_TYPE" = "Gateway" ]; then 
	#	cp -f ../../../Tools/DataBaseScript/init_PersonInfoData.sql ../../../Package/$SERVER_TYPE/FullDeploy/
	#	cp -f ../../../Tools/DataBaseScript/init_SmulateData.sql ../../../Package/$SERVER_TYPE/FullDeploy/
	#fi
	#if [ "$SERVER_TYPE" = "Record" ]; then 
		#cp -f ../../../Tools/ConfigFiles/$SERVER_TYPE/*.xml ../../../Package/$SERVER_TYPE/FullDeploy/full-configuration/
		#cp -f ../../../Tools/ConfigFiles/$SERVER_TYPE/*.xml ../../../Package/$SERVER_TYPE/BigIncrementalUpgrade/20230830001-laster/upgrade-configuration/
		#cp -f ../../../Tools/ConfigFiles/$SERVER_TYPE/*.xml ../../../Package/$SERVER_TYPE/IncrementalUpgrade/20230830001-laster/upgrade-configuration/
		
		#cp -f ../../../Tools/DataBaseScript/init_Database.sql ../../../Package/$SERVER_TYPE/FullDeploy/
		
		#cp -f ../../../Tools/DataBaseScript/upgrade_icc.sql ../../../Package/$SERVER_TYPE/BigIncrementalUpgrade/20230830001-laster/
		#cp -f ../../../Tools/DataBaseScript/upgrade_icc.sql ../../../Package/$SERVER_TYPE/IncrementalUpgrade/20230830001-laster/
	#fi
	
	
	#mv  ../../../Package/$SERVER_TYPE/BigIncrementalUpgrade/20230830001-laster  ../../../Package/$SERVER_TYPE/BigIncrementalUpgrade/$(date '+%Y%m%d')001-laster
	#mv ../../../Package/$SERVER_TYPE/IncrementalUpgrade/20230830001-laster  ../../../Package/$SERVER_TYPE/IncrementalUpgrade/$(date '+%Y%m%d')001-laster


	cd CommandCenter.ICC.$SERVER_TYPE/
	tar -zcf "CommandCenter.ICC.$SERVER_TYPE.tar.gz" "CommandCenter.ICC.$SERVER_TYPE/" "Tools/"
	rm -rf CommandCenter.ICC.$SERVER_TYPE/
	rm -rf Tools/
}

echo "...Begin Commpress Package"
commpress_pkg Server
commpress_pkg Gateway
commpress_pkg CTI
commpress_pkg DeliveryServer
commpress_pkg Record
cd ..
rm -rf Server/
rm -rf Gateway/
rm -rf CTI/
rm -rf DeliveryServer/
rm -rf Record/
rm -rf Tools/
	
echo "...End Build, Package Path: $PACKAGE_PATH/$BUILD_TYPE/"

