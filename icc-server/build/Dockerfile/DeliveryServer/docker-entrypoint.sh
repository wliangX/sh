#!/bin/bash
    
DIR="/usr/CommandCenter/icc/CommandCenter.ICC.DeliveryServer"
FILE=`find $DIR -maxdepth 1 -name 'CommandCenter.ICC.*'`
SERVER_NAME=${FILE##*/}
SERVERTYPE=${DIR##*.}

\cp -ru /usr/CommandCenter/icc/Tools/ConfigFiles/bak/$SERVERTYPE/* /usr/CommandCenter/icc/Tools/ConfigFiles/$SERVERTYPE/

export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

# 查找配置文件具体挂载位置
curl -X GET -m 5 "${UCMS_ADDR}/v1/health/getAddr" -o "/opt/config-center.env"
nacosConfig=`find / -type f -name config-center.env`

#获取Nacos的相关配置信息
NACOS_ADDR=`awk -F= 'NR==1{print $2}' ${nacosConfig}`
NACOS_NAMESPACE=`awk -F= 'NR==2{print $2}' ${nacosConfig}`
NACOS_GROUP=`awk -F= 'NR==3{print $2}' ${nacosConfig}`

# 执行检查次数
EXEC_COUNT=100
# 重试等待时间
WAIT_TIME=10s
for i in `seq $EXEC_COUNT`
do
	echo '尝试检查基础服务状态，第'$i'次'
	curl http://$NACOS_ADDR/nacos/v1/ns/operator/servers?healthy=true -o /opt/nacos.json -s
  NACOS_STATUS=`sed -r 's/.*\"state\":\"([A-Z]+)\".*/\1\ /' /opt/nacos.json`
  echo "nacos服务状态${NACOS_STATUS}"
	if [ $NACOS_STATUS == "UP" ]
	then
		echo "基础服务状态正常！"
		break
  else
    echo "基础服务状态异常！"
	fi
	sleep $WAIT_TIME
done

bash /usr/CommandCenter/icc/config-first-init.sh
bash /usr/CommandCenter/icc/container-restart.sh

echo "--start ..."
echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo "Server name=$SERVER_NAME"

# 启动定时任务
cron && ./$SERVER_NAME
