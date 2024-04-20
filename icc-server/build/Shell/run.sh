#! /bin/bash

if [ -z $1 ]; then
	echo "Fail:  You must specify one parameter!"
	echo "Example:  ./run.sh start[stop]"
	exit 1
fi

DIR=$(cd $(dirname $0); pwd)
FILE=`find $DIR -maxdepth 1 -name 'CommandCenter.ICC.*'`
SERVER_NAME=${FILE##*/}

export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

start(){
    echo "--start ..."
	echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
    ./$SERVER_NAME &
    pid=$(ps -ef|grep $SERVER_NAME |grep -v grep |awk '{print $2}')
    if [ $pid ]; then
        echo "pid=$pid"
    fi  
}
 
stop(){
    pid=$(ps -ef|grep $SERVER_NAME |grep -v grep |awk '{print $2}')
    if [ ! $pid ]; then
        echo "--service is not exist ..."
    else
        kill -9 $pid
    fi  
}
 
check(){ 
    while true
    do
        pid=$(ps -ef|grep $SERVER_NAME |grep -v grep |awk '{print $2}')
        if [[ $pid -eq 0 ]]; then
            echo "--$SERVER_NAME not run, restart ..."
            start 
        fi
		
		echo "--$SERVER_NAME is running ..."
        sleep 30s 
    done
} 
 
case $1 in
start):
    start
    ;;
stop):
	echo "--$SERVER_NAME stopped ..."
    stop
    ;;
check):
    echo "--check ..."
    check
    ;;
*):
    echo "error ..."
    ;;
esac
 
exit 0
