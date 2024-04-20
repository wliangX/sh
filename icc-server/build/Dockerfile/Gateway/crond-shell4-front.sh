#!/bin/sh

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
curl -X GET -m 5 "$1?dataId=$2&group=$3&tenant=$4" > /dev/null
if [[ $? -eq 0 ]]; then
    tempresult=$(curl -X GET "$1?dataId=$2&group=$3&tenant=$4")
    if [ "$(echo $tempresult | grep "<em>Faithfully yours, nginx.</em>")" == "" ] && [ "$tempresult" != "config data not exist" ]; then
	    tempNacosFilePath=$(eval echo '$'file_$(echo -n "$2${3//%3A/:}" |md5sum |cut -d" " -f1))
	    curl -X GET "$1?dataId=$2&group=$3&tenant=$4" -o ${tempNacosFilePath}
	    getXml ${UCMS_ADDR}/v1/cs/configs $2 $3 ${tempNacosFilePath/yml/xml}
    else
	    echo "get nacos config fail"
    fi
else
    echo "get nacos config fail"
fi
}

# 封装监听接口参数
NACOS_PARAMS="";
while read line
do
# 忽略#开头行
if [[ $(expr "$line" : '\#') == 1 ]];then
    continue;
# 忽略空行
elif [[ "${#line}" -eq 0 ]]; then
    continue;
else
    # 文件MD5
    FILE_MD5="";
    # nacos dataid
    nacosDataId=$( echo ${line} | cut -d ',' -f1 | sed "s/^[ \t]*//g");
    # nacos group
    nacosGroup=$( echo ${line} | cut -d ',' -f2 | sed "s/^[ \t]*//g");
    # nacos配置文件存放路径
    configFilePath="$(cd $(dirname $0);pwd)$( echo ${line} | cut -d ',' -f3 | sed "s/^[ \t]*//g")${nacosDataId}";

    # 将文件md5和dataid关联
    temp_nacosDataId=$(echo -n "${nacosDataId}${nacosGroup}" |md5sum |cut -d" " -f1)
    eval file_${temp_nacosDataId}="${configFilePath}"

    # 如果文件不存在则md5随便赋值
    if [[ ! -f "${configFilePath}" ]]; then
	    echo "file not exit" > "${configFilePath}";
	    FILE_MD5="sdf";
    else
	    FILE_MD5=$(md5sum ${configFilePath} | cut -d ' ' -f1 | sed "s/^[ \t]*//g");
    fi

    # 拼接请求监听接口参数
    if [[ "${#NACOS_PARAMS}" -eq 0 ]]; then
	    NACOS_PARAMS="$( echo ${line} | cut -d ',' -f1 | sed "s/^[ \t]*//g")"%02"$( echo ${line} | cut -d ',' -f2 | sed "s/^[ \t]*//g")"%02$FILE_MD5%02"$NACOS_NAMESPACE"%01
    else
	    NACOS_PARAMS=$NACOS_PARAMS"$( echo ${line} | cut -d ',' -f1 | sed "s/^[ \t]*//g")"%02"$( echo ${line} | cut -d ',' -f2 | sed "s/^[ \t]*//g")"%02$FILE_MD5%02"$NACOS_NAMESPACE"%01
    fi
fi
done < "$(cd $(dirname $0);pwd)/nacos-file-config.conf"
# 监听接口返回值
RESULT=$(curl -s  -X POST -H "Long-Pulling-Timeout:30000" "${NACOS_ADDR_URL}/nacos/v1/cs/configs/listener" -d "Listening-Configs=$NACOS_PARAMS")
splitStrNum=`expr $( echo "${RESULT}" |grep -o "%01"|wc -l) `
# 读取监听接口返回值
for i in $(seq 1 $(expr $splitStrNum))
do
# 配置文件信息
tempConfigFileInfo=$(echo ${RESULT//%01/=} | cut -d '=' -f$i | sed "s/^[ \t]*//g");
# 配置文件dataid
CONFIG_FILE_DATAID=$(echo ${tempConfigFileInfo//%02/" "} | cut -d ' ' -f1 | sed "s/^[ \t]*//g");
# 配置文件group
CONFIG_FILE_GROUP=$(echo ${tempConfigFileInfo//%02/" "} | cut -d ' ' -f2 | sed "s/^[ \t]*//g");
# 配置文件namespcase
CONFIG_FILE_NAMESPCASE=$(echo ${tempConfigFileInfo//%02/" "} | cut -d ' ' -f3 | sed "s/^[ \t]*//g");
# 查询nacos配置文件方法
getNacos ${NACOS_ADDR_URL}/nacos/v1/cs/configs ${CONFIG_FILE_DATAID} ${CONFIG_FILE_GROUP} ${CONFIG_FILE_NAMESPCASE}
done
