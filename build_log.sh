#########################################################################
# File Name: build_log.sh
# Author: wl

#创建临时目录来处理日志数据 zip 包
rm -rf temp_log
mkdir -p temp_log

#将现场的压缩文件解压到临时目录
unzip *cti*.zip -d temp_log
cd temp_log
#创建临时处理日志文件的目录
mkdir -p log_file

# 递归遍历 主备服务目录下面所有的压缩日志包 
for file_name in `find . -name '*.zip'`
do
	# 解压到临时文件目录
	unzip $file_name -d log_file
done
# 创建 文本处理文件 并清空
> call_over_log.txt

# 递归遍历所有的日志文件 查询MQ消息 关键字 "request": "topic_call_over_sync",  结果 输出到 call_over_log.txt 文件
grep -Ehri -A 26 "\"request\": \"topic_call_over_sync\"," log_file/2023-12*.log > call_over_log.txt
# 创建sql 插入语句
str1="insert into public.icc_t_callevent(callref_id, caller_id, called_id, call_direction,"

# 创建sql 脚本文件 并清空
> insert_icc_t_callevent.sql
# 防止有重复的话务ID 插入数据失败 不往下执行
echo "set search_path to icc;" >> insert_icc_t_callevent.sql
# 根据 消息文件 获取所有的话务ID
callref_id=`cat call_over_log.txt | grep callref_id  | sort |uniq | grep 2023121 | awk -F ": " '{print $2}'`

for callid in `echo $callref_id`
do
	# 读取 MQ消息的 每个字段数据 并拼接成 sql 语句
	caller_id=`cat call_over_log.txt | grep -A 14 $callid | grep caller_id | awk -F ": " '{print $2}' | tail -1`
	called_id=`cat call_over_log.txt | grep -A 14 $callid | grep called_id | awk -F ": " '{print $2}' | tail -1`
	call_direction=`cat call_over_log.txt | grep -A 14 $callid | grep call_direction | awk -F ": " '{print $2}' | tail -1`
	dial_time=`cat call_over_log.txt | grep -A 14 $callid | grep dial_time | awk -F ": " '{print $2}' | tail -1`
	incoming_time=`cat call_over_log.txt | grep -A 14 $callid | grep incoming_time | awk -F ": " '{print $2}' | tail -1`
	ring_time=`cat call_over_log.txt | grep -A 14 $callid | grep ring_time | awk -F ": " '{print $2}' | tail -1`
	ringback_time=`cat call_over_log.txt | grep -A 14 $callid | grep ringback_time | awk -F ": " '{print $2}' | tail -1`
	release_time=`cat call_over_log.txt | grep -A 14 $callid | grep release_time | awk -F ": " '{print $2}' | tail -1`
	talk_time=`cat call_over_log.txt | grep -A 14 $callid | grep talk_time | awk -F ": " '{print $2}' | tail -1`
	hangup_time=`cat call_over_log.txt | grep -A 14 $callid | grep hangup_time | awk -F ": " '{print $2}' | tail -1`
	hangup_type=`cat call_over_log.txt | grep -A 14 $callid | grep hangup_type | awk -F ": " '{print $2}' | tail -1`
	switch_type=`cat call_over_log.txt | grep -A 15 $callid | grep switch_type | awk -F ": " '{print $2}' | tail -1`
	agent=`cat call_over_log.txt | grep -B 5 $callid | grep agent | awk -F ": " '{print $2}' | tail -1`
	acd=`cat call_over_log.txt | grep -B 5 $callid | grep acd | awk -F ": " '{print $2}' | tail -1`
	original_acd=`cat call_over_log.txt | grep -B 5 $callid | grep original_acd | awk -F ": " '{print $2}' | tail -1`
	acd_dept=`cat call_over_log.txt | grep -B 5 $callid | grep acd_dept | awk -F ": " '{print $2}' | tail -1`
	year=`echo $callid | cut -c 1-5`
	month=`echo $callid | cut -c 6-7`
	day=`echo $callid | cut -c 8,9`
	hour=`echo $callid | cut -c 10,11`
	min=`echo $callid | cut -c 12,13`
	sec=`echo $callid | cut -c 14,15`
	update_time="$year-$month-$day $hour:$min:$sec\""
	str1="insert into public.icc_t_callevent(callref_id, caller_id, called_id, call_direction,"
	str2="($callid $caller_id  $called_id $call_direction"
	echo $update_time
	if [[ $dial_time != "\"\"," ]]; then
		str1="$str1 dial_time,"
		str2="$str2 $dial_time"
	fi
	if [[ $incoming_time != "\"\"," ]]; then
		str1="$str1 incoming_time,"
		str2="$str2 $incoming_time"
	fi
	if [[ $ring_time != "\"\"," ]]; then
		str1="$str1 ring_time,"
		str2="$str2 $ring_time"
	fi
	if [[ $ringback_time != "\"\"," ]]; then
		str1="$str1 ringback_time,"
		str2="$str2 $ringback_time"
	fi
	#if [[ $release_time != "\"\"," ]]; then
	#	str1="$str1 release_time,"
	#	str2="$str2 $release_time"
	#fi
	if [[ $talk_time != "\"\"," ]]; then
		str1="$str1 talk_time,"
		str2="$str2 $talk_time"
	fi
	if [[ $hangup_time != "\"\"," ]]; then
		str1="$str1 hangup_time,"
		str2="$str2 $hangup_time"
	fi
	# 拼接成 sql 语句
	#str1="insert into public.icc_t_callevent(callref_id, caller_id, called_id, call_direction, dial_time, incoming_time , ring_time , ringback_time , talk_time, hangup_time, hangup_type, acd, create_time)"
	echo "$str1 hangup_type, switch_type, acd, original_acd, acd_dept, update_time) values $str2 $hangup_type $switch_type $acd $original_acd $acd_dept $update_time);;;" >> insert_icc_t_callevent.sql
	#echo "insert into public.icc_t_callevent(callref_id, caller_id, called_id, call_direction, dial_time, incoming_time , ring_time , ringback_time , talk_time, hangup_time, hangup_type, acd, create_time) values ($callid $caller_id  $called_id $call_direction $dial_time $incoming_time $ring_time $ringback_time $talk_time $hangup_time $hangup_type $acd $create_time);;;" >> insert_icc_t_callevent.sql

done

sed -i "s/\"/'/g" insert_icc_t_callevent.sql
#sed -i "s/,);;;/);;;/g" insert_icc_t_callevent.sql
