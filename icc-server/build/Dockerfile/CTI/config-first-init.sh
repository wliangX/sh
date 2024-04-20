#!/bin/bash
    
# 查找配置文件
nacosConfig=`find / -type f -name config-center.env`
# nacos地址
NACOS_ADDR_URL=`awk -F= 'NR==1{print $2}' ${nacosConfig}`
# nacos命名空间
NACOS_NAMESPACE=`awk -F= 'NR==2{print $2}' ${nacosConfig}`

# 查询公共配置并格式转换
function getXml() {
# 不是公共项目无需转换xml
if [ "$(echo $2 | grep "public-command-")" != "" ] && [ "$(echo $2 | grep "yml")" != "" ]; then
    curl -X GET -m 5 "$1?dataId=$2&group=$3&tenant=${NACOS_NAMESPACE}" > /dev/null
    if [[ $? -eq 0 ]]; then
	tempresult=$(curl -X GET "$1?dataId=$2&group=$3&tenant=${NACOS_NAMESPACE}")
	if [ "$(echo $tempresult | grep "<em>Faithfully yours, nginx.</em>")" == "" ] && [ "$tempresult" != "config data not exist" ]; then
	    curl -X GET "$1?dataId=$2&group=$3&tenant=${NACOS_NAMESPACE}" -o $4
	else
	    echo "get xml config fail"
	fi
    else
	echo "get xml config fail"
    fi
fi
}

# 查询公共配置并格式转换
function getNacos() {
curl -X GET -m 5 "$1?dataId=$2&group=$3&tenant=${NACOS_NAMESPACE}" > /dev/null
if [[ $? -eq 0 ]]; then
    tempresult=$(curl -X GET "$1?dataId=$2&group=$3&tenant=${NACOS_NAMESPACE}")
    if [ "$(echo $tempresult | grep "<em>Faithfully yours, nginx.</em>")" == "" ] && [ "$tempresult" != "config data not exist" ]; then
	curl -X GET "$1?dataId=$2&group=$3&tenant=${NACOS_NAMESPACE}" -o $4
	getXml ${UCMS_ADDR}/v1/cs/configs $2 $3 ${configFilePath/yml/xml}
    else
	echo "get nacos config fail"
    fi
else
    echo "get nacos config fail"
fi
}

# 获得当前项目配置文件信息
while read line
do
# 忽略#开头行
if [[ $(expr "$line" : '\#') == 1 ]];then
    continue;
# 忽略空行
elif [[ "${#line}" -eq 0 ]]; then
    continue;
else
    # nacos dataid
    nacosDataId=$( echo ${line} | cut -d ',' -f1 | sed "s/^[ \t]*//g");
    # nacos group
    nacosGroup=$( echo ${line} | cut -d ',' -f2 | sed "s/^[ \t]*//g");
    # nacos配置文件存放路径
    configFilePath="$(cd $(dirname $0);pwd)$( echo ${line} | cut -d ',' -f3 | sed "s/^[ \t]*//g")${nacosDataId}";
    # 查询nacos配置文件方法
    getNacos ${NACOS_ADDR_URL}/nacos/v1/cs/configs ${nacosDataId} ${nacosGroup} ${configFilePath}
fi
done < "$(cd $(dirname $0);pwd)/nacos-file-config.conf"
