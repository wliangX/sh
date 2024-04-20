#! /bin/bash



DIR=$(cd $(dirname $0); pwd)
FILE=`find $DIR -maxdepth 1 -name 'CommandCenter.ICC.*'`
SERVER_NAME=${FILE##*/}
SERVERTYPE=${DIR##*.}

\cp -ru /usr/CommandCenter/icc/Tools/ConfigFiles/bak/$SERVERTYPE/* /usr/CommandCenter/icc/Tools/ConfigFiles/$SERVERTYPE/ 

export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

# 调用定时器方法
sh /usr/CommandCenter/icc/crondShell4front.sh

echo "--start ..."
echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo "Server name=$SERVER_NAME"
cron && ./$SERVER_NAME