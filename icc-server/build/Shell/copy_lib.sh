#!/bin/bash

CURRENT_DIR=$(cd $(dirname $0); pwd)
ICC_ROOT=$(cd $(dirname $0);cd ../..; pwd)

echo "...ICC_ROOT: $ICC_ROOT"

############################################################################################
#amq
AMQ_DIR="$ICC_ROOT/ThirdParty/Linux/activemq"
AMQ_INC_DIR="$AMQ_DIR/activemq-cpp/src/main"
AMQ_LIB_DIR="$AMQ_DIR/lib"

APR_DIR="$AMQ_DIR/apr/"
APR_INI_DIR="$AMQ_DIR/apr/include/"
APR_ICONV_DIR="$AMQ_DIR/apr-iconv/"
APR_ICONV_INC_DIR="$AMQ_DIR/apr-iconv/include/"
APR_UTIL_DIR="$AMQ_DIR/apr-util/"
APR_UTIL_INC_DIR="$AMQ_DIR/apr-util/include/"
APR_EXPAT_DIR="$AMQ_DIR/expat/"

rm -rf "$AMQ_DIR"
mkdir -p "$AMQ_INC_DIR"
mkdir -p "$AMQ_LIB_DIR"

mkdir -p "$APR_INI_DIR"
mkdir -p "$APR_ICONV_INC_DIR"
mkdir -p "$APR_UTIL_INC_DIR"

cp -rf "/usr/local/activemq-cpp/include/activemq-cpp-3.9.4/." "$AMQ_INC_DIR"
cp -rf "/usr/local/activemq-cpp/lib/." "$AMQ_LIB_DIR"

find "/usr/local/activemq-cpp/lib/"     -type f -name "libactivemq-cpp.so*" -exec cp -rf {} 	"$AMQ_LIB_DIR/" ";"
find "/usr/local/activemq-cpp/lib/"     -type l -name "libactivemq-cpp.so*" -exec cp -rf {} 	"$AMQ_LIB_DIR/" ";"

find "/usr/local/activemq-cpp/apr/lib/" -type f -name "libapr-1.so*" -exec cp -rf {} 			"$AMQ_LIB_DIR/" ";"
find "/usr/local/activemq-cpp/apr/lib/" -type l -name "libapr-1.so*" -exec cp -rf {} 			"$AMQ_LIB_DIR/" ";"

cp -rf "/usr/local/activemq-cpp/apr/include/apr-1/." "$APR_INI_DIR"
cp -rf "/usr/local/activemq-cpp/apr-iconv/include/apr-1/." "$APR_ICONV_INC_DIR"
cp -rf "/usr/local/activemq-cpp/aprutil/include/apr-1/." "$APR_UTIL_INC_DIR"

###########################################################################################
#boost
BOOST_DIR="$ICC_ROOT/ThirdParty/Linux/boost/"
BOOST_LIB_DIR="$BOOST_DIR/stage/lib/"

rm -rf "$BOOST_DIR"
mkdir -p "$BOOST_LIB_DIR"

cp -rf "/usr/local/boost/include/boost/" "$BOOST_DIR"
cp -rf "/usr/local/boost/lib/." "$BOOST_LIB_DIR"


############################################################################################
#postgres
PG_DIR="$ICC_ROOT/ThirdParty/Linux/postgres/"
PG_INC_DIR="$PG_DIR/include/"
PG_LIB_DIR="$PG_DIR/lib/"

rm -rf "$PG_DIR"
mkdir -p "$PG_INC_DIR/catalog/"
mkdir -p "$PG_LIB_DIR"

files=`find /usr/local/postgres/include/ -maxdepth 1 -name '*.h'`
for fc in $files; do
	cp -f "$fc" "$PG_INC_DIR"
done

cp -rf "/usr/local/postgres/include/server/catalog/." "$PG_INC_DIR/catalog/"
cp -rf "/usr/local/postgres/include/server/catalog/pg_type.h" "$PG_INC_DIR/"
cp -rf "/usr/local/postgres/lib/." "$PG_LIB_DIR"

############################################################################################
#redis
REDIS_SRC="/root/redis/acl"
REDIS_DIR="$ICC_ROOT/ThirdParty/Linux/redis"
REDIS_LIB_DIR="$REDIS_DIR/lib"

rm -rf "$REDIS_DIR"
mkdir -p "$REDIS_LIB_DIR"

cp -rf "$REDIS_SRC/lib_acl/" "$REDIS_DIR"	
cp -rf "$REDIS_SRC/lib_acl_cpp/" "$REDIS_DIR"
cp -rf "$REDIS_SRC/lib_protocol/" "$REDIS_DIR"

cp -rf "/usr/local/redis/lib/." "$REDIS_LIB_DIR"

############################################################################################
#rapidjson
JOSON_SRC="/root/rapidjson"
JOSON_DIR="$ICC_ROOT/ThirdParty/Linux"
rm -rf "$JOSON_SRC"
mkdir -p "$JOSON_SRC"

cp -rf "$REDIS_SRC" "$JOSON_DIR"
