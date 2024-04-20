#!/bin/sh
    
# 查找配置文件
nacosConfig=`find / -type f -name config-center.env`
# nacos命名空间
NACOS_NAMESPACE=`awk -F= 'NR==2{print $2}' ${nacosConfig}`

# 请求参数
PARAMS="";   
# 获取配置文件信息                                             
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
    # 封装请求参数                                   
    PARAMS=${PARAMS}"{\"namespace\":\"${NACOS_NAMESPACE}\",\"dataId\":\"${nacosDataId}\",\"group\":\"${nacosGroup}\"},"
fi                                                                                                                               
done < `find / -type f -name nacos-file-config.conf` 

# 获得容器id
head -1 /proc/self/cgroup|cut -d/ -f3|while read line                                                                                
do               
# 保存容器与配置信息                                                                                                                    
curl -X POST -m 5 "${UCMS_ADDR}/v1/configCenter/container/add" -d "{\"containerId\":\"$line\",\"nacosInfos\":[${PARAMS%?}]}" --header "Content-Type: application/json"
done
