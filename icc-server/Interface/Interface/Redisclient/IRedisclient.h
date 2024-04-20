#pragma once
//STL
#include <map>
#include <string>
#include <vector>

namespace ICC
{
	namespace Redis
	{
		class IRedisClient :
			public IResource
		{
		public:
			/*
			* 初始化，必须调用
			* @参数 p_strAddress
			*	   redis 服务地址，集群部署地址以英文标点“ ; ”隔开
			* @参数 p_bClusterMode
			*	   是否集群，是 true ，否 false
			* @参数 p_iKeyTTL
			*	   p_strKey 的默认生存时间，单位（秒）
			* @返回 false:	失败
			*	   true:    操作成功
			*/
			virtual bool Connect(const std::string& p_strAddress, bool p_bClusterMode, int p_iKeyTTL) = 0;
			/*
			* 释放资源，退出时必须调用
			* @参数 void
			* @返回 void
			*/
			virtual void Disconnect() = 0;
			/*
			*  服务锁,该锁基于SetNx实现
			* @参数 p_pcKey
			*	   字符串对象的 key
			* @参数 p_strLockValue
			*	   锁成功后，p_pcKey 对应的 value
			* @参数 p_iLockTTL
			*	   锁成功后，锁的生存时间
			* @参数 p_iTimeOut
			*	   请求锁的超时时间，在p_iTimeOut秒内未获得锁返回失败
			* @参数 p_strError
			*	   返回的错误提示信息
			* @返回 false:	失败
			*	   true:    操作成功
			*/
			virtual bool ServiceLock(const std::string& p_pcKey, std::string& p_strLockValue, int p_iLockTTL, int p_iTimeOut, std::string& p_strError) = 0;			
			
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*									   Redis键key接口											   */
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*
			* 判断KEY是否存在
			* @参数 p_strKey
			*	   Redis数据库的键字符串
			* @返回 false:	表示出错或不存在
			*	   true:    表示存在
			*/
			virtual bool Exists(const std::string& p_strKey) = 0;
			/*
			* 查找所有符合给定模式pattern的key
			* @参数 p_strKeyPattern
			*	   匹配模式
			* @参数 p_vecOutKeys
			*	   非NULL时用来存储结果集
			* @返回 结果集的数量
			*      0  为空
			*      <0 表示出错
			*      >0 匹配数量
			* 匹配模式举例：
			*   KEYS * 匹配数据库中所有 key 。
			*   KEYS h?llo 匹配 hello ， hallo 和 hxllo 等。
			*   KEYS h*llo 匹配 hllo 和 heeeeello 等。
			*   KEYS h[ae]llo 匹配 hello 和 hallo ，但不匹配 hillo 。
			*/
			virtual int KeysPattern(const std::string& p_strKeyPattern, std::vector<std::string>* p_vecOutKeys) = 0;
			/*
			* 删除一个 KEY
			* @参数 p_strKey
			*	   Redis数据库的键字符串
			* @返回 0: 未删除任何 KEY
			*	   -1: 出错
			*	   > 0: 真正删除的 KEY 的个数，该值有可能少于输入的 KEY 的个数
			*/
			virtual int Del(const std::string& p_strKey) = 0;
			/*
			* 设置 KEY 的生存周期，单位（秒）
			* @参数 p_strKey
			*	   Redis数据库的键字符串
			* @参数 p_iTTL
			*	   生存周期（秒）
			* @返回 > 0: 成功设置了生存周期
			*	   0：该 p_strKey 不存在
			*	   < 0 : 出错
			*/
			virtual int Expire(const std::string& p_strKey, const int p_iTTL) = 0;
			/*
			* 获得 KEY 的剩余生存周期，单位（秒）
			* @参数 p_strKey
			*	   Redis数据库的键字符串
			* @返回 > 0: 该 p_strKey 剩余的生存周期（秒）
			*	   -2：p_strKey 不存在
			*	   -1：当 p_strKey 存在但没有设置剩余时间
			*/
			virtual int TTL(const std::string& p_strKey) = 0;

			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*									   Redis事务接口											   */
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*
			* WATCH乐观锁，在EXEC命令执行之前,监视任意数量的数据库键，并在EXEC命令执行时，检查被监视的键是否至少有一个已经被修改过，
			* 如果是的话，服务器将拒绝执行事务,并向客户端返回代表事务执行失败的空回复
			* @参数 strKey
			*      键空间的键.
			* @返回 false:	失败
			*	   true:    操作成功
			*/
			virtual bool Watch(const std::vector<std::string>& strkeys) = 0;
			/*
			* 取消 WATCH 命令对所有key的监视
			* @参数 void
			* @返回 false:	失败
			*	   true:    操作成功
			*/
			virtual bool UnWatch(void) = 0;
			/*
			* 标记一个事务块的开始，事务块内的多条命令会按照先后顺序被放进一个队列当中，最后由EXEC命令原子性地执行
			* @参数 void
			* @返回 false:	失败
			*	   true:    操作成功
			*/
			virtual bool Multi(void) = 0;
			/*
			* 执行所有事务块内的命令，假如某个(或某些)key正处于WATCH命令的监视之下，且事务块中有和这个(或这些)key相关的命令，
			* 那么EXEC命令只在这个(或这些)key没有被其他命令所改动的情况下执行并生效，否则该事务被打断(abort)；在执行本条命令
			* 成功后，可以调用下面的GetSize()/GetChild()获得每条命令的操作结果
			* @参数 void
			* @返回 false:	失败
			*	   true:    操作成功
			*/
			virtual bool Exec(void) = 0;
			/*
			* 取消事务，放弃执行事务块内的所有命令，如果正在使用WATCH命令监视某个(或某些)key，那么取消所有监视，等同于执行命令UNWATCH
			* @参数 void
			* @返回 false:	失败
			*	   true : 操作成功
			*/
			virtual bool Discard(void) = 0;
			/*
			* 在成功调用 exec 后调用本函数获得操作结果数组的长度
			* @参数 void
			* @返回 false:	失败
			*	   true : 操作成功
			*/
			virtual unsigned int GetSize(void) = 0;
			/*
			* 获取指定下标的对应的命令的执行结果对象
			* get the result of the given subscript
			* @参数 i
			*      命令执行结果在结果数组中的下标
			* @参数 strOut
			*      执行某条命令的结果
			* @参数 cmd
			*      该参数非空时存放对应的redis命令
			* @返回 
			*      当i越界时返回false
			*/
			virtual bool GetChild(unsigned int i, std::string& strOut, std::string* strCmd = NULL) = 0;

			virtual std::vector<std::string> GetCommands(void) = 0;

			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*									   Redis字符串对象string接口								   */
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*
			* 设置string对象键值对
			* @参数 strKey
			*      键空间的键.
			* @参数 strValue
			*      键空间的值string对象.
			* @返回 false:	表示出错或该key对象非字符串对象
			*	   true:    设置成功
			*/
			virtual bool Set(const std::string& strKey, const std::string& strValue) = 0;

			/*
			* 设置string对象键值对，带过期时间的
			* @参数 strKey
			*      键空间的键.
			* @参数 strValue
			*      键空间的值string对象.
			* @返回 false:	表示出错或该key对象非字符串对象
			*	   true:    设置成功
			*/
			virtual bool SetEx(const std::string& strKey, const std::string& strValue, unsigned int uTimeoutSeconds) = 0;

			/*
			* 返回key所关联的字符串值
			* @参数 strKey
			*      有序集键值.
			* @参数 strValue
			*      键空间的值string对象.
			* @返回 false:	表示出错或该key对象非字符串对象
			*	   true:    设置成功
			*/
			virtual bool Get(const std::string& strKey, std::string& strValue) = 0;

			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*									   Redis哈希对象hash接口									   */
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*
			* 设置p_pcKey所对应的哈希中p_strField字段的值为p_strValue
			* @参数 p_pcKey/p_strKey
			*      哈希对象的key.
			* @参数 p_strField
			*      哈希对象的域名称.
			* @参数 p_strValue
			*      哈希对象的域值.
			* @返回 false:	出错
			*	   true:    成功
			*/
			virtual bool HSet(const char* p_pcKey, const char* p_strField, const char* p_strValue) = 0;
			virtual bool HSet(const std::string& p_strKey, const std::string& p_strField, std::string& p_strValue) = 0;
			/*
			* 将多个"域-值"对添加至 KEY 对应的哈希表中
			* @参数 p_strKey
			*      哈希对象的key.
			* @参数 p_mapItem
			*      哈希对象的域-值对.
			* @返回 false:	出错
			*	   true:    成功
			*/
			virtual bool HMSet(const std::string& p_strKey, const std::map<std::string, std::string>& p_mapItem) = 0;
			/*
			* 从redis哈希表中获取某个 key 对象的某个域的值
			* @参数 p_strKey
			*      哈希对象的key.
			* @参数 p_strField
			*      哈希对象的域名称.
			* @参数 p_strValue
			*      哈希对象的域值.
			* @返回 false:	出错
			*	   true:    成功
			*/
			virtual bool HGet(const std::string& p_strKey, const std::string& p_strField, std::string& p_strValue) = 0;
			/*
			* 从redis哈希表中获取某个key对象的所有域字段的值
			* @参数 p_strKey
			*      哈希对象的key.
			* @参数 p_mapItem
			*      存储域字段名-值查询结果集.
			* @返回 false:	出错
			*	   true:    成功
			*/
			virtual bool HGetAll(const std::string& p_strKey, std::map<std::string, std::string>& p_mapItem) = 0;

			virtual bool HGetAllEx(const std::string& p_strKey, std::map<std::string, std::string>& p_mapItem) = 0;

			/*
			* 从redis哈希表中删除某个key对象的某些域字段
			* @参数 p_strKey
			*      哈希对象的key.
			* @参数 p_pcField
			*      域字段名称.
			* @返回 false:	出错
			*	   true:    成功
			*/
			virtual bool HDel(const std::string& p_strKey, const std::string& p_pcField) = 0;
			/*
			* 从redis哈希表中删除某个key对象的某些域字段
			* @参数 p_strKey
			*      哈希对象的key.
			* @参数 p_pcField
			*      域字段名称,最后一个字段必须是NULL
			* @返回 false:	出错
			*	   true:    成功
			*/
			virtual bool HDelFields(const std::string& p_strKey, const char* p_pcField, ...) = 0;
			/*
			* 从redis哈希表中删除某个key对象的某些域字段
			* @参数 p_strKey
			*      哈希对象的key.
			* @参数 p_vecFields
			*      域字段名称集合.
			* @返回 false:	出错
			*	   true:    成功
			*/
			virtual bool HDelFields(const std::string& p_strKey, const std::vector<std::string>& p_vecFields) = 0;
			/*
			* 从redis哈希表中检查某个key对象的某个域字段是否存在
			* @参数 p_strKey
			*      哈希对象的key.
			* @参数 p_pcField
			*      域字段名称.
			* @返回 false:	表明出错或该key对象非哈希对象或该域字段不存在
			*	   true:    存在
			*/
			virtual bool HExists(const std::string& p_strKey, const std::string& p_pcField) = 0;
			/*
			* 以迭代方式遍布哈希p_pcKey中的键值对
			* @参数 p_strKey
			*      哈希对象的key.
			* @参数 p_iCursor
			*      游标值，初次遍历必须传入0，之后以函数返回的游标遍历.
			* @参数 p_mapOut
			*      存储得到的结果集，内部以追加的方式添加结果，可在调用该函数前后清空以免溢出.
			* @参数 p_strPattern
			*      匹配模式，nullptr无效
			* @参数 p_uiCount
			*      限定的结果集数量，非空指针时有效.
			* @返回 下一游标位置
			*      0:遍历结束
			*      -1：出错
			*      >0 游标的下一位置
			*/
			virtual int HScan(const std::string& p_strKey, int p_iCursor, std::map<std::string, std::string>& p_mapOut, const std::string& p_strPattern = "", const unsigned int* p_uiCount = nullptr) = 0;
			
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*									   Redis列表对象接口										   */
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*
			* 返回指定列表对象的元素个数
			* @参数 p_strKey
			*      列表对象的key
			* @返回 指定列表对象的长度（即元素个数）
			*	   -1: 发生错误  
			*/
			virtual int LLen(const std::string& p_strKey) = 0;
			/*
			* 将一个或多个值元素插入到列表对象的表尾
			* @参数 p_strKey
			*      列表对象的key
			* @参数 p_vecValues
			*      元素值集合
			* @返回 添加完后当前列表对象中的元素个数,当该键不存在时会添加新的列表对象及对象中的元素
			*	   -1: 表示出错或该对象非列表对象
			*/
			virtual int RPush(const std::string& p_strKey, const std::vector<std::string>& p_vecValues) = 0;
			/*
			* 返回对应的列表对象中，指定下标的元素
			* @参数 p_strKey
			*      列表对象的key
			* @参数 p_uiIndex
			*      下标值
			* @参数 p_strResult
			*      存储结果
			* @返回 true表明操作成功，此时若p_strResult数据非空则表明正确获得了指定下标的元素，
			*      如果p_strResult.empty()表示没有获得元素；返回false时表明操作失败
			*/
			virtual bool LIndex(const std::string& p_strKey, unsigned int p_uiIndex, std::string& p_strResult) = 0;
			/*
			* 返回列表中指定区间内（闭区间）的元素，区间以偏移量p_iStart和p_iEnd指定；下标起始值从0开始，-1表示最后一个下标值
			* @参数 p_strKey
			*      列表对象的key
			* @参数 p_iStart
			*      起始下标值
			* @参数 p_iEnd
			*      结束下标值
			* @参数 p_vesResult
			*      存储列表对象中指定区间的元素集合
			* @返回 true 成功
			*      false 表示出错或p_strKey非列表对象
			*/
			virtual bool LRange(const std::string& p_strKey, int p_iStart, int p_iEnd, std::vector<std::string>& p_vesResult) = 0;
			/*
			* 将列表下标为idx的元素的值设置为value，当idx参数超出范围，或对一个空列表(key不存在)进行lset时，返回一个错误
			* @参数 p_strKey
			*      列表对象的key
			* @参数 p_uiIndex
			*      下标位置，当为负值时则从尾部向头尾部定位，否则采用顺序方式；
			*      如：0 表示头部第一个元素，-1 表示尾部开始的第一个元素
			* @参数 p_strValue
			*      元素新值
			* @返回 true 成功
			*      false key非列表对象或key不存在或idx超出范围
			*/
			virtual bool LSet(const std::string& p_strKey, unsigned int p_uiIndex, const std::string& p_strValue) = 0;
			/*
			* 根据元素值从列表对象中移除指定数量的元素
			* @参数 p_strKey
			*      列表对象的key
			* @参数 p_iCount
			*      移除元素的数量限制，p_iCount的含义如下
			*      p_iCount > 0 : 从表头开始向表尾搜索，移除与p_strValue相等的元素，数量为p_iCount
			*      p_iCount < 0 : 从表尾开始向表头搜索，移除与p_strValue相等的元素，数量为p_iCount的绝对值
			*      p_iCount = 0 : 移除表中所有与p_strValue相等的值
			* @参数 p_strValue
			*      指定的元素值，需要从列表对象中遍历所有与该值比较
			* @返回 被移除的对象数量，返回值含义如下：
			*      -1： 出错或该 p_strKey 对象非列表对象
			*      0：  key不存在或移除的元素个数为 0
			*      >0： 被成功移除的元素数量
			*/
			virtual int LRem(const std::string& p_strKey, int p_iCount, const std::string& p_strValue) = 0;

			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*									   Redis集合set对象接口								       */
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*
			* 将一个或多个member元素加入到集合p_strKey当中，已经存在于集合的member元素将被忽略;
			* @参数 p_strKey
			*      集合对象的key
			* @参数 p_vecMembers
			*      集合成员
			* @返回 被添加到集合中的新元素的数量，不包括被忽略的元素
			*      -1: 表示出错或p_strKey非集合对象
			*/
			virtual int SAdd(const std::string& p_strKey, const std::vector<std::string>& p_vecMembers) = 0;
			/*
			* 获得集合对象中成员的数量
			* @参数 p_strKey
			*      集合对象的key
			* @返回 返回该集合对象中成员数量，含义如下
			*      -1: 出错或非集合对象
			*      0： 成员数量为空或该key不存在
			*      > 0：成员数量非空
			*/
			virtual int SCard(const std::string& p_strKey) = 0;
			/*
			* 返回集合中的所有成员
			* @参数 p_strKey
			*      集合对象的key
			* @参数 p_vecResult
			*      存储结果集
			* @返回 结果集数量
			*      -1: 出错或非集合对象
			*/
			virtual int SMembers(const std::string& p_strKey, std::vector<std::string>& p_vecResult) = 0;
			/*
			* 移除集合中的一个或多个member元素，不存在的member元素会被忽略
			* @参数 p_strKey
			*      集合对象的key
			* @参数 p_vecMembers
			*      需要被移除的成员列表
			* @返回 被移除的成员元素的个数
			*      -1: 当出错或非集合对象
			*      0:  key不存在或成员不存在
			*/
			virtual int SRem(const std::string& p_strKey, const std::vector<std::string>& p_vecMembers) = 0;
			/*
			* 判断元素是否为集合中的元素
			* @参数 p_strKey
			*      集合对象的key
			* @参数 p_strMember
			*      给定值
			* @返回 true  是
			*      false 不是或者出错或非集合对象
			*/
			virtual bool SIsMember(const std::string& p_strKey, const std::string& p_strMember) = 0;
			/*
			* 求所有给定集合的差集
			* @参数 p_strDstKey
			*      存储结果集
			* @参数 p_strFirstKey
			*      集合1对象的键值
			* @参数 p_strSecondKey
			*      集合2对象的键值
			* @返回 结果集数量
			*      -1 表示出错或有一个非集合对象不是或者出错或非集合对象
			*/
			virtual int SDiffStore(const std::string& p_strDstKey, const std::string& p_strFirstKey, const std::string& p_strSecondKey) = 0;
			/*
			* 求所有给定集合的交集
			* @参数 p_strDstKey
			*      存储结果集
			* @参数 p_strFirstKey
			*      集合1对象的键值
			* @参数 p_strSecondKey
			*      集合2对象的键值
			* @返回 结果集数量
			*      -1 表示出错或有一个非集合对象不是或者出错或非集合对象
			*/
			virtual int SInterStore(const std::string& p_strDstKey, const std::string& p_strFirstKey, const std::string& p_strSecondKey) = 0;
			/*
			* 求所有给定集合的并集
			* @参数 p_strDstKey
			*      存储结果集
			* @参数 p_strFirstKey
			*      集合1对象的键值
			* @参数 p_strSecondKey
			*      集合2对象的键值
			* @返回 结果集数量
			*      -1 表示出错或有一个非集合对象不是或者出错或非集合对象
			*/
			virtual int SUnionStore(const std::string& p_strDstKey, const std::string& p_strFirstKey, const std::string& p_strSecondKey) = 0;
			
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*									   Redis有序集zset接口									   */
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*
			* 添加对应 key 的有序集
			* @参数 strKey
			*      有序集键值.
			* @参数 vMembers
			*      "分值-成员"集合.
			* @返回 0:	表示一个也未添加，可能因为该成员已经存在于有序集中
			*	   -1:	表示出错或 key 对象非有序集对象
			*	   >0:	新添加的成员数量
			*/
			virtual int ZAdd(const std::string strKey, const std::vector<std::pair<const char*, double> >&vMembers) = 0;
			/*
			* 获得相应键的有序集的成员数量
			* @参数 strKey
			*      有序集键值..
			* @返回 0:	该键不存在
			*	   -1:	出错或该键的数据对象不是有效的有序集对象
			*	   >0:	当前键值对应的数据对象中的成员个数
			*/
			virtual int ZCard(const std::string strKey) = 0;
			/*
			* 获得 key 的有序集中指定分值区间的成员个数
			* @参数 strKey
			*      有序集键值.
			* @参数 dMin
			*      最小分值.
			* @参数 dMax
			*      最大分值.
			* @返回 0:	该键对应的有序集不存在或该 KEY 有序集的对应分值区间成员为空
			*	   -1:	出错或该键的数据对象不是有效的有序集对象
			*	   >0:	符合条件的成员个数
			*/
			virtual int ZCount(const std::string strKey, double dMin, double dMax) = 0;
			/*
			* 将 key 的有序集中的某个成员的分值加上增量inc
			* @参数 strKey
			*      有序集键值.
			* @参数 dInc
			*      增量值.
			* @参数 strMember
			*      有序集中成员名.
			* @参数 dResult
			*      非空时存储结果值.
			* @返回 true:	操作成功
			*	   false:	操作失败
			*/
			virtual bool ZIncrby(const std::string strKey, double dInc, const std::string strMember, double* pResult = NULL) = 0;

			virtual bool ZIncrby(const std::string strKey, double dInc, const std::string strMember, unsigned int sLen, double* pResult = NULL) = 0;
			/*
			* 从 key 的有序集中获得指定位置区间的成员名列表，成员按分值递增方式排序
			* @参数 strKey
			*      有序集键值.
			* @参数 nStart
			*      起始下标位置.
			* @参数 nStop
			*      结束下标位置（结果集同时含该位置）.
			* @参数 vResult
			*      非空时存储结果值,内部先调用result.clear()清除其中的元素.
			* @返回 0:	表示结果集为空或 key 不存在
			*	   -1:  表示出错或 key 对象非有序集对象
			*	   >0:  结果集的数量
			*
			*注：对于下标位置，0 表示第一个成员，1 表示第二个成员；-1 表示最后一个成员，-2 表示倒数第二个成员，以此类推
			*
			*操作成功后可以通过以下任一方式获得数据
			*1、在调用方法中传入非空的存储结果对象的地址
			*2、基类方法get_value获得指定下标的元素数据
			*3、基类方法 get_child 获得指定下标的元素对象(redis_result）然后再通过
			*   redis_result::argv_to_string 方法获得元素数据
			*	redis_result::argv_to_string 方法获得元素数据
			*4、基类方法 get_result 方法取得总结果集对象 redis_result，然后再通过
			*	redis_result::get_child 获得一个元素对象，然后再通过方式 2 中指定的方法获得该元素的数据
			*5、基类方法 get_children 获得结果元素数组对象，再通过
			*	redis_result 中的方法argv_to_string从每一个元素对象中获得元素数据
			*/
			virtual int ZRange(const std::string strKey, int nStart, int nStop, std::vector<std::string>* vResult) = 0;
			/*
			* 从 key 的有序集中获得指定位置区间的成员名及分值列表，成员按分值递增方式排序
			* @参数 strKey
			*      有序集键值.
			* @参数 nStart
			*      起始下标位置.
			* @参数 nStop
			*      结束下标位置（结果集同时含该位置）.
			* @参数 vOut
			*      存储 "成员名-分值对"结果集，内部先调用 out.clear().
			* @返回 0:	表示结果集为空或 key 不存在
			*	   -1:  表示出错或 key 对象非有序集对象
			*	   >0:  结果集的数量
			*
			*注：对于下标位置，0 表示第一个成员，1 表示第二个成员；-1 表示最后一个成员，-2 表示倒数第二个成员，以此类推
			*/
			virtual int ZRangeWithScores(std::string strKey, int nStart, int nStop, std::vector<std::pair<std::string, double>>& vOut) = 0;
			/*
			* 从有序集中删除某个成员
			* @参数 strKey
			*      有序集键值.
			* @参数 vMembers
			*      要删除的成员列表.
			* @返回 0:	表示该有序集不存在或成员不存在
			*	   -1:  表示出错或该 key 非有序集对象
			*	   >0:  表示成功删除的成员数量
			*/
			virtual int ZRem(std::string strKey, const std::vector<std::string>& vMembers) = 0;
			/*
			* 从 key 的有序集中获得指定位置区间的成员名列表，成员按分值递减方式排序
			* @参数 strKey
			*      有序集键值.
			* @参数 nStart
			*      起始下标位置.
			* @参数 nStop
			*      结束下标位置（结果集同时含该位置）.
			* @参数 pResult
			*      非空时存储结果集.
			* @返回 0:	表示该有序集不存在或成员不存在
			*	   -1:  表示出错或该 key 非有序集对象
			*	   >0:  表示成功删除的成员数量
			*
			* 注：对于下标位置，0 表示第一个成员，1 表示第二个成员；-1 表示最后一个成员，-2 表示倒数第二个成员，以此类推
			*/
			virtual int ZRevrange(std::string strKey, int nStart, int nStop, std::vector<std::string>* pResult) = 0;
			/*
			* 从 key 的有序集中获得指定位置区间的成员名及分值列表，成员按分值递减方式排序
			* @参数 strKey
			*      有序集键值.
			* @参数 nStart
			*      起始下标位置.
			* @参数 nStop
			*      结束下标位置（结果集同时含该位置）.
			* @参数 vOut
			*      存储 "成员名-分值对"结果集，内部先调用 out.clear().
			* @返回 0:	表示该有序集不存在或成员不存在
			*	   -1:  表示出错或该 key 非有序集对象
			*	   >0:  表示成功删除的成员数量
			*
			* 注：对于下标位置，0 表示第一个成员，1 表示第二个成员；-1 表示最后一个成员，-2 表示倒数第二个成员，以此类推
			*/
			virtual int ZRevrangeWithScores(std::string strKey, int nStart, int nStop, std::vector<std::pair<std::string, double>>& vOut) = 0;



			/*
			*  服务锁,该锁基于SetNx实现
			* @参数 p_pcKey
			*	   字符串对象的 key
			* @参数 p_strLockValue
			*	   锁成功后，p_pcKey 对应的 value
			* @参数 p_iLockTTL
			*	   锁成功后，锁的生存时间
			* @参数 p_iTimeOut
			*	   请求锁的超时时间，在p_iTimeOut秒内未获得锁返回失败
			* * @参数 p_bOperatorLock
			*	   是否操作锁
			* @参数 p_strError
			*	   返回的错误提示信息
			* @返回 false:	失败
			*	   true:    操作成功
			*/
			virtual bool ServiceLockEx(const std::string& p_pcKey, std::string& p_strLockValue, int p_iLockTTL, int p_iTimeOut, bool p_bOperatorLock, std::string& p_strError) = 0;
		};

		typedef boost::shared_ptr<IRedisClient> IRedisClientPtr;
	}
}

#define ICCIRedisClientResourceName "ICC.Com.RedisClient.ResourceName"

#define ICCSetIRedisClient(ImplClass) \
this->GetResourceManager()->SetResource(ICCIRedisClientResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIRedisClientNull \
this->GetResourceManager()->SetResource(ICCIRedisClientResourceName, nullptr) \

#define ICCGetIRedisClient() \
boost::dynamic_pointer_cast<Redis::IRedisClient>(this->GetResourceManager()->GetResource(ICCIRedisClientResourceName))