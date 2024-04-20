#!/bin/bash

##//设置脚本中所需命令的执行路径
export PATH="/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin"
if [ "$?" != 0 ];then
	##//echo 为输出到屏幕
	echo "Please check env PATH"
	##//exit 0 为强制终止脚本
	exit 0
fi

DIR=$(cd $(dirname $0); pwd)
##//name执行的脚本文件的全名
name=$(basename $0)
SHELL_NAME=${name%.sh*}
#echo "SHELL_NAME: $SHELL_NAME"

SERVER_NAME=${SHELL_NAME%.Install*}
#echo "SERVER_NAME: $SERVER_NAME"

CONTAINER_TYPE=server
if [ "$SERVER_NAME" == "CommandCenter.ICC.Server" ]; then
	CONTAINER_TYPE=server
elif [ "$SERVER_NAME" == "CommandCenter.ICC.Gateway"  ]; then
	CONTAINER_TYPE=gateway	
elif [ "$SERVER_NAME" == "CommandCenter.ICC.CTI"  ]; then
	CONTAINER_TYPE=cti		
elif [ "$SERVER_NAME" == "CommandCenter.ICC.Record"  ]; then
	CONTAINER_TYPE=record
else
	echo "Unknown server name: $SERVER_NAME, exit"
	exit 1
fi

##echo "CONTAINER_TYPE: $CONTAINER_TYPE"

##//本地目录
LOCAL_PATH=/usr/CommandCenter
##//SERVER_PORT服务端口
SERVER_PORT="11111"
##防火墙开放端口
SERVER_TCP_PORTS="29012"
SERVER_TCP_HTTPPORTS="21010"
GATEWAY_TCP_PORTS="29014"
GATEWAY_TCP_HTTPPORTS="21020"
GATEWAY_UDP_PORTS="21000"
CTI_TCP_PORTS="29016"
CTI_TCP_HTTPPORTS="21030"
RECORD_TCP_HTTPPORTS="21090"

##//SERVER_NAME服务名称
SYSTEM_NAME=icc


##//镜像名
SERVER_DOCKER_NAME=commandcenter/icc/server
GATEWAY_DOCKER_NAME=commandcenter/icc/gateway
CTI_DOCKER_NAME=commandcenter/icc/cti
RECORD_DOCKER_NAME=commandcenter/icc/record
##//容器名
SERVER_CONTAINER_NAME=commandcenter.icc.server
GATEWAY_CONTAINER_NAME=commandcenter.icc.gateway
CTI_CONTAINER_NAME=commandcenter.icc.cti
RECORD_CONTAINER_NAME=commandcenter.icc.record

## //OPTSTATUS: 0卸载 1安装 2停止docker容器 3启动docker容器 4制作镜像 5导出镜像
while getopts ":i:p:u:w:v:s:h" opt
do
    case $opt in
        i)
        echo "ip--$OPTARG"
	PGIP=$OPTARG;
        ;;
        p)
        echo "port--$OPTARG"
	PGPORT=$OPTARG;
        ;;
        u)
        echo "user--$OPTARG"
	PGUSER=$OPTARG;
        ;;
        w)
        echo "pwd--$OPTARG"
	PGPWD=$OPTARG;
        ;;
		v)
        echo "version--$OPTARG"
	SERVERVERSION=$OPTARG;
        ;;
		h)
		echo "-i ip"
		echo "-p port"
		echo "-u user"
		echo "-w password"
		echo "-v vesion"
		echo "-s 0:Uninstall docker 1:Install docker 2:Stop docker 3:Start docker 4:Build docker 5:Save image"
		exit 0
		;;
        s)
        ##echo "OPT--$OPTARG"
	OPTSTATUS=$OPTARG;	
	
	## //OPTSTATUS: 0卸载 1安装 2停止docker容器 3启动docker容器 4制作镜像 5导出镜像
	if [ "$OPTSTATUS" == "0" ]; then
		echo "Uninstall $SERVER_NAME"
	elif [ "$OPTSTATUS" == "1" ]; then
		echo "Install $SERVER_NAME"
	elif [ "$OPTSTATUS" == "2" ]; then
		echo "Stop $SERVER_NAME"
	elif [ "$OPTSTATUS" == "3" ]; then
		echo "Start $SERVER_NAME"
	elif [ "$OPTSTATUS" == "4" ]; then
		echo "Build $SERVER_NAME"
	elif [ "$OPTSTATUS" == "5" ]; then
		echo "Save $SERVER_NAME"		
	else
		echo "Unknown OPT, Use -h for help please!"	
		exit 1	
	fi
	
	read -r -p "Are You Sure? [Y/n] " input
		case $input in
		[yY][eE][sS]|[yY])
			echo $input
			;;
		[nN][oO]|[nN])
			exit 1
       		;;  
		*)
		echo "Invalid input..."		
		exit 1		
		;;
		esac
        ;;
        ?)
        echo "---$opt"
        exit 1;;
    esac
done

##//循环添加端口到防火墙规则
function add_firewall_ports(){
	##PORT_TYPE udp/tcp
	PORT_TYPE=$1
	OPEN_PORTS=$2
	
	#循环添加端口到防火墙规则中
	FIREWALLSTATE=$(firewall-cmd --state -q && echo "true" || echo "false")
	if [ "$FIREWALLSTATE" = "true" ];then
		echo "Firewall is running, Prepare to add ports to firewall"
	else
		echo "Firewall is running, Prepare to starting firewall"
		systemctl start firewalld
	fi
                         
	for i in $OPEN_PORTS
	do
		echo "Adding $i $PORT_TYPE Port to firewall"
		firewall-cmd --permanent --zone=public --add-port=${i}/$PORT_TYPE
	done
	
	echo "Reload firewall"
	firewall-cmd --reload
	
	#if [ "$FIREWALLSTATE" = "false" ];then
	#	echo "Stop firewall service"
	#	systemctl stop firewalld
	#fi
}
##//循环移除防火墙端口规则
function remove_firewall_ports(){
	##PORT_TYPE udp/tcp
	PORT_TYPE=$1
	OPEN_PORTS=$2
	
	#循环添加端口到防火墙规则中
	FIREWALLSTATE=$(firewall-cmd --state -q && echo "true" || echo "false")
	#echo "FIREWALLSTATE" $FIREWALLSTATE
	if [ "$FIREWALLSTATE" = "true" ];then
		echo "Firewall is running, Prepare to remove ports to firewall"
	else
		echo "Firewall is running, Prepare to starting firewall"
		systemctl start firewalld
	fi
                         
	for i in $OPEN_PORTS
	do
		echo "Removeing $i $PORT_TYPE Port from firewall"
		firewall-cmd --permanent --zone=public --remove-port=${i}/$PORT_TYPE
	done
	
	echo "Reload firewall"
	firewall-cmd --reload
	
	#if [ "$FIREWALLSTATE" = "false" ];then
	#	echo "Stop firewall service"
	#	systemctl stop firewalld
	#fi
}

##//添加或移除服务依赖的防火墙端口，带一参数 0 移除，1 添加
function cmd_firewall_ports(){
	CMD_TYPE=$1
	
	if [ "$CMD_TYPE" = "1" ];then
		if [ "$CONTAINER_TYPE" == "server" ];then
			add_firewall_ports tcp $SERVER_TCP_PORTS
			add_firewall_ports tcp $SERVER_TCP_HTTPPORTS
		fi
		
		if [ "$CONTAINER_TYPE" == "gateway" ];then
			add_firewall_ports udp $GATEWAY_UDP_PORTS
			add_firewall_ports tcp $GATEWAY_TCP_PORTS
			add_firewall_ports tcp $GATEWAY_TCP_HTTPPORTS
		fi
		
		if [ "$CONTAINER_TYPE" == "cti" ];then
			add_firewall_ports tcp $CTI_TCP_PORTS
			add_firewall_ports tcp $CTI_TCP_HTTPPORTS
		fi
		
		if [ "$CONTAINER_TYPE" == "record" ];then
			add_firewall_ports tcp $RECORD_TCP_HTTPPORTS
		fi
	else
		if [ "$CONTAINER_TYPE" == "server" ];then
			remove_firewall_ports tcp $SERVER_TCP_PORTS
			remove_firewall_ports tcp $SERVER_TCP_HTTPPORTS
		fi
		
		if [ "$CONTAINER_TYPE" == "gateway" ];then
			remove_firewall_ports udp $GATEWAY_UDP_PORTS
			remove_firewall_ports tcp $GATEWAY_TCP_PORTS
			remove_firewall_ports tcp $GATEWAY_TCP_HTTPPORTS
		fi
		
		if [ "$CONTAINER_TYPE" == "cti" ];then
			remove_firewall_ports tcp $CTI_TCP_PORTS
			remove_firewall_ports tcp $CTI_TCP_HTTPPORTS
		fi
		
		if [ "$CONTAINER_TYPE" == "record" ];then
			remove_firewall_ports tcp $RECORD_TCP_HTTPPORTS
		fi
	fi
}

##//判断服务端口是否被占用 参数一个 要检测的端口
function server_port_status(){
    netstat -lnut | grep $1
    if [ "$?" == 0 ];then
        return 0
    else
        return 1
    fi
}

##//在usr目录下创建“CommandCenter”目录 
function create_command_center_dir(){
    if [ ! -d $LOCAL_PATH ];then
        mkdir $LOCAL_PATH
    fi
	
    if [ ! -d $LOCAL_PATH/$SYSTEM_NAME ];then
		mkdir $LOCAL_PATH/$SYSTEM_NAME
    fi
	
	return 0
}

 ##//在“CommandCenter”目录下创建服务目录 入参一个要在CommandCenter 下创建的目录
function create_server_dir(){
    if [ ! -d $LOCAL_PATH/$SYSTEM_NAME/$CONTAINER_TYPE ];then 
        mkdir $LOCAL_PATH/$SYSTEM_NAME/$CONTAINER_TYPE
    fi
	
	return 0
}
########################################## UNINSTALL FUN ############################################

##//删除服务 入参一个 服务所有文件目录名
function delete_server(){
	if [ "$CONTAINER_TYPE" == "server" ];then
		rm -rf $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/Server/
		rm -rf $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/SDG/Server/
	fi
	
	if [ "$CONTAINER_TYPE" == "gateway" ];then
		rm -rf $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/Gateway/
		rm -rf $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/SDG/Gateway/
	fi
	
	if [ "$CONTAINER_TYPE" == "cti" ];then
		rm -rf $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/CTI/
		rm -rf $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/SDG/CTI/
	fi
	
	if [ "$CONTAINER_TYPE" == "record" ];then
		rm -rf $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/Record/
		rm -rf $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/SDG/Record/
	fi
	
    sleep 2
 }

##//删除CommandCenter目录
function delete_home_path(){
    if [ "'ls -A ${LOCAL_PATH}'" = "" ];
    then 
        rm -rf $LOCAL_PATH
	sleep 2
        echo "progress server uninstalled.." 
    fi
}

##//创建一个容器并运行
function docker_run(){
	VERSION=$1
	
   	if [ "$CONTAINER_TYPE" == "server" ];then
		docker run -d --network host --log-opt max-size=500m --restart=always \
		-v $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/Server/Config.xml:/usr/CommandCenter/$SYSTEM_NAME/Tools/ConfigFiles/Server/Config.xml \
		-v $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/Server/StaticConfig.xml:/usr/CommandCenter/$SYSTEM_NAME/Tools/ConfigFiles/Server/StaticConfig.xml \
		-v $LOCAL_PATH/$SYSTEM_NAME/Logs/:/usr/CommandCenter/$SYSTEM_NAME/Tools/Logs/ \
		-v /etc/localtime:/etc/localtime \
		--name=$SERVER_CONTAINER_NAME $SERVER_DOCKER_NAME:$VERSION	
		
		echo "Process $SERVER_CONTAINER_NAME Installed 100%" 
	fi
	
	if [ "$CONTAINER_TYPE" == "gateway" ];then
		docker run -d --network host --log-opt max-size=500m --restart=always \
		-v $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/Gateway/Config.xml:/usr/CommandCenter/$SYSTEM_NAME/Tools/ConfigFiles/Gateway/Config.xml \
		-v $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/Gateway/StaticConfig.xml:/usr/CommandCenter/$SYSTEM_NAME/Tools/ConfigFiles/Gateway/StaticConfig.xml \
		-v $LOCAL_PATH/$SYSTEM_NAME/Logs/:/usr/CommandCenter/$SYSTEM_NAME/Tools/Logs/ \
		-v /etc/localtime:/etc/localtime \
		--name=$GATEWAY_CONTAINER_NAME $GATEWAY_DOCKER_NAME:$VERSION	
		
		echo "Process $GATEWAY_CONTAINER_NAME Installed 100%"
	fi
	
	if [ "$CONTAINER_TYPE" == "cti" ];then
		docker run -d --network host --log-opt max-size=500m --restart=always \
		-v $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/CTI/Config.xml:/usr/CommandCenter/$SYSTEM_NAME/Tools/ConfigFiles/CTI/Config.xml \
		-v $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/CTI/StaticConfig.xml:/usr/CommandCenter/$SYSTEM_NAME/Tools/ConfigFiles/CTI/StaticConfig.xml \
		-v $LOCAL_PATH/$SYSTEM_NAME/Logs/:/usr/CommandCenter/$SYSTEM_NAME/Tools/Logs/ \
		-v /etc/localtime:/etc/localtime \
		--name=$CTI_CONTAINER_NAME $CTI_DOCKER_NAME:$VERSION

		echo "Process $CTI_CONTAINER_NAME Installed 100%"
	fi
	
	if [ "$CONTAINER_TYPE" == "record" ];then
		docker run -d --network host --log-opt max-size=500m --restart=always \
		-v $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/Record/Config.xml:/usr/CommandCenter/$SYSTEM_NAME/Tools/ConfigFiles/Record/Config.xml \
		-v $LOCAL_PATH/$SYSTEM_NAME/ConfigFiles/Record/StaticConfig.xml:/usr/CommandCenter/$SYSTEM_NAME/Tools/ConfigFiles/Record/StaticConfig.xml \
		-v $LOCAL_PATH/$SYSTEM_NAME/Logs/:/usr/CommandCenter/$SYSTEM_NAME/Tools/Logs/ \
		-v /etc/localtime:/etc/localtime \
		--name=$RECORD_CONTAINER_NAME $RECORD_DOCKER_NAME:$VERSION

		echo "Process $CTI_CONTAINER_NAME Installed 100%"
	fi
	
	echo "Start container completed"
}

##//停止容器
function docker_stop(){	
	if [ "$CONTAINER_TYPE" == "server" ];then
		echo "docker stop $SERVER_CONTAINER_NAME"
		docker stop $SERVER_CONTAINER_NAME
	fi
	
	if [ "$CONTAINER_TYPE" == "gateway" ];then
		echo "docker stop $GATEWAY_CONTAINER_NAME"
		docker stop $GATEWAY_CONTAINER_NAME
	fi	
	
	if [ "$CONTAINER_TYPE" == "cti" ];then
		echo "docker stop $CTI_CONTAINER_NAME"
		docker stop $CTI_CONTAINER_NAME
	fi	
	
	if [ "$CONTAINER_TYPE" == "record" ];then
		echo "docker stop $RECORD_CONTAINER_NAME"
		docker stop $RECORD_CONTAINER_NAME
	fi	
}
##//启动容器
function docker_start(){
	if [ "$CONTAINER_TYPE" == "server" ];then
		echo "docker start $SERVER_CONTAINER_NAME"
		docker start $SERVER_CONTAINER_NAME
	fi
	
	if [ "$CONTAINER_TYPE" == "gateway" ];then
		echo "docker start $GATEWAY_CONTAINER_NAME"
		docker start $GATEWAY_CONTAINER_NAME
	fi
	
	if [ "$CONTAINER_TYPE" == "cti" ];then
		echo "docker start $CTI_CONTAINER_NAME"
		docker start $CTI_CONTAINER_NAME
	fi

	if [ "$CONTAINER_TYPE" == "record" ];then
		echo "docker start $RECORD_CONTAINER_NAME"
		docker start $RECORD_CONTAINER_NAME
	fi
}

##//删除镜像
function docker_rm(){
	if [ "$CONTAINER_TYPE" == "server" ];then
		echo "docker rm -f $SERVER_CONTAINER_NAME"
		docker rm -f $SERVER_CONTAINER_NAME
				
		IMAGES_ID=`get_image_id $CONTAINER_TYPE`
		if [ -n "$IMAGES_ID" ];then			
			echo "docker rmi $IMAGES_ID"
			docker rmi -f $IMAGES_ID	
		fi	
	fi
	
	if [ "$CONTAINER_TYPE" == "gateway" ];then
		echo "docker rm -f $GATEWAY_CONTAINER_NAME"
		docker rm -f $GATEWAY_CONTAINER_NAME
		
		IMAGES_ID=`get_image_id $CONTAINER_TYPE`		
		if [ -n "$IMAGES_ID" ];then
			echo "docker rmi $IMAGES_ID"
			docker rmi -f $IMAGES_ID	
		fi
	fi
	
	if [ "$CONTAINER_TYPE" == "cti" ];then
		echo "docker rm -f $CTI_CONTAINER_NAME"
		docker rm -f $CTI_CONTAINER_NAME
		
		IMAGES_ID=`get_image_id $CONTAINER_TYPE`		
		if [ -n "$IMAGES_ID" ];then
			echo "docker rmi $IMAGES_ID"
			docker rmi -f $IMAGES_ID	
		fi
	fi
	
	if [ "$CONTAINER_TYPE" == "record" ];then
		echo "docker rm -f $RECORD_CONTAINER_NAME"
		docker rm -f $RECORD_CONTAINER_NAME
		
		IMAGES_ID=`get_image_id $CONTAINER_TYPE`		
		if [ -n "$IMAGES_ID" ];then
			echo "docker rmi $IMAGES_ID"
			docker rmi -f $IMAGES_ID	
		fi
	fi
}

##//加载镜像
function docker_load(){

	IMAGES_ID=`get_image_id $CONTAINER_TYPE`	
	if [ -n "$IMAGES_ID" ];then
		echo "docker rmi $IMAGES_ID"
		docker rmi $IMAGES_ID	
	fi
		
	if [ "$CONTAINER_TYPE" == "server" ];then			
		echo "docker load commandcenter.icc.server.tar"
		docker load --input commandcenter.icc.server.tar
	fi

	if [ "$CONTAINER_TYPE" == "gateway" ];then	
		echo "docker load commandcenter.icc.gateway.tar"
		docker load --input commandcenter.icc.gateway.tar
	fi
	
	if [ "$CONTAINER_TYPE" == "cti" ];then	
		echo "docker load commandcenter.icc.cti.tar"
		docker load --input commandcenter.icc.cti.tar
	fi

	if [ "$CONTAINER_TYPE" == "record" ];then	
		echo "docker load commandcenter.icc.record.tar"
		docker load --input commandcenter.icc.record.tar
	fi
}

##//制作镜像
function docker_build(){
	VERSION=$1
	
	echo "docker build -t commandcenter/$SYSTEM_NAME/$CONTAINER_TYPE:$VERSION ."
	docker build -t commandcenter/$SYSTEM_NAME/$CONTAINER_TYPE:$VERSION .
}

##//导出镜像
function docker_save(){
	VERSION=$1
	
	if [ "$CONTAINER_TYPE" == "server" ];then
		echo "docker save -o commandcenter.icc.server.tar commandcenter/$SYSTEM_NAME/$CONTAINER_TYPE:$VERSION"
		docker save -o commandcenter.icc.server.tar commandcenter/$SYSTEM_NAME/$CONTAINER_TYPE:$VERSION
	fi
	
	if [ "$CONTAINER_TYPE" == "gateway" ];then
		echo "docker save -o commandcenter.icc.gateway.tar commandcenter/$SYSTEM_NAME/$CONTAINER_TYPE:$VERSION"
		docker save -o commandcenter.icc.gateway.tar commandcenter/$SYSTEM_NAME/$CONTAINER_TYPE:$VERSION
	fi

	if [ "$CONTAINER_TYPE" == "cti" ];then
		echo "docker save -o commandcenter.icc.cti.tar commandcenter/$SYSTEM_NAME/$CONTAINER_TYPE:$VERSION"
		docker save -o commandcenter.icc.cti.tar commandcenter/$SYSTEM_NAME/$CONTAINER_TYPE:$VERSION
	fi	
	
	if [ "$CONTAINER_TYPE" == "record" ];then
		echo "docker save -o commandcenter.icc.record.tar commandcenter/$SYSTEM_NAME/$CONTAINER_TYPE:$VERSION"
		docker save -o commandcenter.icc.record.tar commandcenter/$SYSTEM_NAME/$CONTAINER_TYPE:$VERSION
	fi	
}

function get_image_id(){
	if [ -z $CONTAINER_TYPE ]; then
		echo "Fail:  You must specify image name!"
		exit 1
	fi
	
	DOCKER_INFO=""
	
	if [ "$CONTAINER_TYPE" == "server" ];then
		DOCKER_INFO=$(docker images | grep commandcenter/$SYSTEM_NAME/server)	
	fi
	
	if [ "$CONTAINER_TYPE" == "gateway" ];then
		DOCKER_INFO=$(docker images | grep commandcenter/$SYSTEM_NAME/gateway)
	fi
	
	if [ "$CONTAINER_TYPE" == "cti" ];then
		DOCKER_INFO=$(docker images | grep commandcenter/$SYSTEM_NAME/cti)
	fi
	
	if [ "$CONTAINER_TYPE" == "record" ];then
		DOCKER_INFO=$(docker images | grep commandcenter/$SYSTEM_NAME/record)
	fi
	
	if [ -n "$DOCKER_INFO" ];then
		arrary=(`echo $DOCKER_INFO | tr '\t' ' '`)	
		
		if [ ${#arrary[@]} -lt 3 ];then
			echo "get docker info faied"
			exit ""
		fi

		IMAGES_ID=${arrary[2]}
	fi
	
	echo $IMAGES_ID
}

function get_server_version(){
	if [  -z $SERVERVERSION ]; then
		if [ -f version ];then
			line=$(head -n 1 version)
			SERVERVERSION=$line
		else
			echo "Version file is not exist, input version opt likes -v 1.0.1 please"
			exit 0        
		fi
    else
		SERVERVERSION="V$SERVERVERSION"
    fi
	
	echo "Server Version: $SERVERVERSION"
}

########################################## MAIN FUN ############################################
## //OPTSTATUS: 0卸载 1安装 2停止docker容器 3启动docker容器 4制作镜像 5导出镜像
if [ $OPTSTATUS == '1' ];then	

	##//判断是否输入版本信息
	get_server_version
    if [  -z $SERVERVERSION ]; then
		echo "Server's version is null, exit"
		exit 0        
    fi
	
	##//判断端口是否被占用
	if [ "$CONTAINER_TYPE" == "gateway" ]; then
		server_port_status $SERVER_PORT
		if [ "$?" == 0 ];then
			echo "error $SERVER_PORT is occupied! finished"
			exit 1
		fi
	fi
	
	##开放防火墙策略
	##cmd_firewall_ports 1
		
	##//创建CommandCenter目录
	create_command_center_dir
	##//拷贝配置文件
	cp -rf ConfigFiles/ $LOCAL_PATH/$SYSTEM_NAME/
	sleep 2	

	docker_load

	##//创建docker容器 [server/gateway]
	docker_run $SERVERVERSION
	
	#echo "server instation completed" 
	
##//如果用户输入0的情况下执行卸载程序
elif [ $OPTSTATUS == '0' ];then 

	##//停掉docker容器
	docker_rm

	##//删除服务
	delete_server

	##//删除CommandCenter目录
	delete_home_path
	sleep 2
	
	##关闭防火墙策略
	##cmd_firewall_ports 0
	
	echo "Progress Uninstall Finished 100%" 
elif [ $OPTSTATUS == '2' ];then
	docker_stop
	echo "docker stoped"
elif [ $OPTSTATUS == '3' ];then
	docker_start
	echo "docker running" 
elif [ $OPTSTATUS == '4' ];then
	##//判断是否输入版本信息
	get_server_version
    if [  -z $SERVERVERSION ]; then
		echo "Server version is null, exit"
		exit 0        
    fi
	
	docker_build $SERVERVERSION
	echo "docker build" 
elif [ $OPTSTATUS == '5' ];then
	##//判断是否输入版本信息
	get_server_version
    if [  -z $SERVERVERSION ]; then
		echo "Server version is null, exit"
		exit 0        
    fi
	
	docker_save $SERVERVERSION
	echo "docker save"
else
	echo "Fail:  You must specify one parameter!"
	echo "Example: ./docker.sh -s 0 (0 install, 1 uninstall, 2 start, 3 stop, 4 build, 5 save)"
	exit 1
fi