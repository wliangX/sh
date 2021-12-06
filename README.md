#!/bin/bash

high_light="\033[47;31;1m"
color_end="\033[0m"
work_dir=$(cd $(dirname $0);pwd)
function usage() {
  echo -e "Usage: $0 -h host_specifier [-d] [-m s|b|a] [-i url] [-t timeout] [-v 8|11]"
  echo -e "such as:  -h rbf65022, must"
  echo -e "          -d debug, don't restart DUT when test failed"
  echo -e "          -s 1|0, optional and default is 0, 0: open boot, 1: secure boot\n"
  echo -e "          -m s, optional and default, which means streaming mode\n" \
          "            anything except s a b, means the three mode will be setup,\n" \
          "            but if you run this bkc, it will run streaming mode."
  echo -e "          -v 8|11, optional, default is 11, 8 for NVR8, 11 for NVR11"
  echo -e "          -t 1500, optional and default, which means running 1500 ${high_light}minutes${color_end}"
  echo -e "          -i url_of_bkc, optional and no default, if there was no this option,\n" \
          "           the test will start to run right now"
  echo -e "${high_light}Notes${color_end}: when you give -i option, the setup will begin firstly, and when the setup completed,\n" \
    "      this script will ask you whether you want to run this bkc right now.\n" \
    "      by default, this script will run right now,\n" \
    "      if you want to stop to run, then you need type  'n' or 'N'"
  exit 0
}

warnning_echo() {
  echo -e "\033[47;31;5m${1}\033[0m"
}

MACHINES="machines.txt"
if [ ! -e $MACHINES ]; then
  echo "lack of machines.txt file"
  useage
fi
debug=0
while getopts "h:dm:t:u:i:v:s:S:c:C:T:" opt; do
  case $opt in
  i) url=$OPTARG ;;
  h) host=$OPTARG ;;
  d) debug=1;;
  m) mode=$OPTARG ;;
  t) timeout=$OPTARG ;;
  v) nvr8_11=$OPTARG;;
  s) secure=$OPTARG;;
  S) silver_case=$OPTARG;;
  c) config=$OPTARG;;
  T) To=$OPTARG;;
  C) CC=$OPTARG;;
  esac
done
shift $(($OPTIND - 1))

# [ -z $host ] && usage
if [ -z $host ]; then
  ifconfig | grep -w inet | awk '{print $2}' >ip_tmp
  while read nuc_ip; do
    if grep -q $nuc_ip $MACHINES; then
      host=$nuc_ip
    fi
  done <ip_tmp
  rm -f ip_tmp
  if [ -z $host ];then
    echo "[Error] Cannot find machine information in machines.txt"
    exit 1
  fi
fi

timeout=${timeout:-1500}
nvr8_11=${nvr8_11:-11}
secure=${secure:-0}
video=${video:-0}
[ -z $(echo $timeout | sed 's/[[:digit:]]//g') ] || usage
mode=${mode:-s}
[ -z $(echo $mode | sed 's/[abs0]//g') ] || usage
[ $nvr8_11 != 8 -a $nvr8_11 != 11 ] && usage

[ $secure != 0 -a $secure != 1 ] && usage

case $mode in
s) mode="streaming" ;;
b) mode="bypass" ;;
a) mode="advance" ;;
*) mode="all" ;;
esac
if [ $mode == "bypass" ]; then
       if [ $nvr8_11 == 11 ]; then
               config=${config:-nvr11_1_hevc.json}
       fi
       if [ $nvr8_11 == 8 ]; then
               config=${config:-nvr8_48_hevc.json}
       fi
elif [ $mode == "streaming" ]; then
       config=${config:-s48_hevc_nvr8.json}
elif [ $mode == "advance" ]; then
       config=${config:-nvr8_48_hevc.json}
else
       warnning_echo "mode error"
fi

ia_host_num=$(grep $host $MACHINES | wc -l)
ia_host_ip=$(grep $host $MACHINES | awk -F: '{print $2}')
ia_host_user=$(grep $host $MACHINES | awk -F: '{print $3}')
ia_host_pwd=$(grep $host $MACHINES | awk -F: '{print $4}')

if [ $ia_host_num != 1 ] || [ -z $ia_host_ip ] || [ -z $ia_host_user ] || [ -z $ia_host_pwd ]; then
  warnning_echo "Please confirm the machines.txt about this $host."
  usage
fi
# remove all system log before run test cases
# sudo rm -rf /var/log/*

tag=$(echo $url |grep -E '^http')

if [ ! -z $tag ]; then
  echo "Install python packages."
  sudo python3 -m pip  install --upgrade pip
  sudo python3 -m pip install -r ${work_dir}/requirements.txt
