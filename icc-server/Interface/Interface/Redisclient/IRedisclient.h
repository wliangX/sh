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
			* ��ʼ�����������
			* @���� p_strAddress
			*	   redis �����ַ����Ⱥ�����ַ��Ӣ�ı�㡰 ; ������
			* @���� p_bClusterMode
			*	   �Ƿ�Ⱥ���� true ���� false
			* @���� p_iKeyTTL
			*	   p_strKey ��Ĭ������ʱ�䣬��λ���룩
			* @���� false:	ʧ��
			*	   true:    �����ɹ�
			*/
			virtual bool Connect(const std::string& p_strAddress, bool p_bClusterMode, int p_iKeyTTL) = 0;
			/*
			* �ͷ���Դ���˳�ʱ�������
			* @���� void
			* @���� void
			*/
			virtual void Disconnect() = 0;
			/*
			*  ������,��������SetNxʵ��
			* @���� p_pcKey
			*	   �ַ�������� key
			* @���� p_strLockValue
			*	   ���ɹ���p_pcKey ��Ӧ�� value
			* @���� p_iLockTTL
			*	   ���ɹ�����������ʱ��
			* @���� p_iTimeOut
			*	   �������ĳ�ʱʱ�䣬��p_iTimeOut����δ���������ʧ��
			* @���� p_strError
			*	   ���صĴ�����ʾ��Ϣ
			* @���� false:	ʧ��
			*	   true:    �����ɹ�
			*/
			virtual bool ServiceLock(const std::string& p_pcKey, std::string& p_strLockValue, int p_iLockTTL, int p_iTimeOut, std::string& p_strError) = 0;			
			
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*									   Redis��key�ӿ�											   */
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*
			* �ж�KEY�Ƿ����
			* @���� p_strKey
			*	   Redis���ݿ�ļ��ַ���
			* @���� false:	��ʾ����򲻴���
			*	   true:    ��ʾ����
			*/
			virtual bool Exists(const std::string& p_strKey) = 0;
			/*
			* �������з��ϸ���ģʽpattern��key
			* @���� p_strKeyPattern
			*	   ƥ��ģʽ
			* @���� p_vecOutKeys
			*	   ��NULLʱ�����洢�����
			* @���� �����������
			*      0  Ϊ��
			*      <0 ��ʾ����
			*      >0 ƥ������
			* ƥ��ģʽ������
			*   KEYS * ƥ�����ݿ������� key ��
			*   KEYS h?llo ƥ�� hello �� hallo �� hxllo �ȡ�
			*   KEYS h*llo ƥ�� hllo �� heeeeello �ȡ�
			*   KEYS h[ae]llo ƥ�� hello �� hallo ������ƥ�� hillo ��
			*/
			virtual int KeysPattern(const std::string& p_strKeyPattern, std::vector<std::string>* p_vecOutKeys) = 0;
			/*
			* ɾ��һ�� KEY
			* @���� p_strKey
			*	   Redis���ݿ�ļ��ַ���
			* @���� 0: δɾ���κ� KEY
			*	   -1: ����
			*	   > 0: ����ɾ���� KEY �ĸ�������ֵ�п������������ KEY �ĸ���
			*/
			virtual int Del(const std::string& p_strKey) = 0;
			/*
			* ���� KEY ���������ڣ���λ���룩
			* @���� p_strKey
			*	   Redis���ݿ�ļ��ַ���
			* @���� p_iTTL
			*	   �������ڣ��룩
			* @���� > 0: �ɹ���������������
			*	   0���� p_strKey ������
			*	   < 0 : ����
			*/
			virtual int Expire(const std::string& p_strKey, const int p_iTTL) = 0;
			/*
			* ��� KEY ��ʣ���������ڣ���λ���룩
			* @���� p_strKey
			*	   Redis���ݿ�ļ��ַ���
			* @���� > 0: �� p_strKey ʣ����������ڣ��룩
			*	   -2��p_strKey ������
			*	   -1���� p_strKey ���ڵ�û������ʣ��ʱ��
			*/
			virtual int TTL(const std::string& p_strKey) = 0;

			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*									   Redis����ӿ�											   */
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*
			* WATCH�ֹ�������EXEC����ִ��֮ǰ,�����������������ݿ��������EXEC����ִ��ʱ����鱻���ӵļ��Ƿ�������һ���Ѿ����޸Ĺ���
			* ����ǵĻ������������ܾ�ִ������,����ͻ��˷��ش�������ִ��ʧ�ܵĿջظ�
			* @���� strKey
			*      ���ռ�ļ�.
			* @���� false:	ʧ��
			*	   true:    �����ɹ�
			*/
			virtual bool Watch(const std::vector<std::string>& strkeys) = 0;
			/*
			* ȡ�� WATCH ���������key�ļ���
			* @���� void
			* @���� false:	ʧ��
			*	   true:    �����ɹ�
			*/
			virtual bool UnWatch(void) = 0;
			/*
			* ���һ�������Ŀ�ʼ��������ڵĶ�������ᰴ���Ⱥ�˳�򱻷Ž�һ�����е��У������EXEC����ԭ���Ե�ִ��
			* @���� void
			* @���� false:	ʧ��
			*	   true:    �����ɹ�
			*/
			virtual bool Multi(void) = 0;
			/*
			* ִ������������ڵ��������ĳ��(��ĳЩ)key������WATCH����ļ���֮�£�����������к����(����Щ)key��ص����
			* ��ôEXEC����ֻ�����(����Щ)keyû�б������������Ķ��������ִ�в���Ч����������񱻴��(abort)����ִ�б�������
			* �ɹ��󣬿��Ե��������GetSize()/GetChild()���ÿ������Ĳ������
			* @���� void
			* @���� false:	ʧ��
			*	   true:    �����ɹ�
			*/
			virtual bool Exec(void) = 0;
			/*
			* ȡ�����񣬷���ִ��������ڵ���������������ʹ��WATCH�������ĳ��(��ĳЩ)key����ôȡ�����м��ӣ���ͬ��ִ������UNWATCH
			* @���� void
			* @���� false:	ʧ��
			*	   true : �����ɹ�
			*/
			virtual bool Discard(void) = 0;
			/*
			* �ڳɹ����� exec ����ñ�������ò����������ĳ���
			* @���� void
			* @���� false:	ʧ��
			*	   true : �����ɹ�
			*/
			virtual unsigned int GetSize(void) = 0;
			/*
			* ��ȡָ���±�Ķ�Ӧ�������ִ�н������
			* get the result of the given subscript
			* @���� i
			*      ����ִ�н���ڽ�������е��±�
			* @���� strOut
			*      ִ��ĳ������Ľ��
			* @���� cmd
			*      �ò����ǿ�ʱ��Ŷ�Ӧ��redis����
			* @���� 
			*      ��iԽ��ʱ����false
			*/
			virtual bool GetChild(unsigned int i, std::string& strOut, std::string* strCmd = NULL) = 0;

			virtual std::vector<std::string> GetCommands(void) = 0;

			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*									   Redis�ַ�������string�ӿ�								   */
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*
			* ����string�����ֵ��
			* @���� strKey
			*      ���ռ�ļ�.
			* @���� strValue
			*      ���ռ��ֵstring����.
			* @���� false:	��ʾ������key������ַ�������
			*	   true:    ���óɹ�
			*/
			virtual bool Set(const std::string& strKey, const std::string& strValue) = 0;

			/*
			* ����string�����ֵ�ԣ�������ʱ���
			* @���� strKey
			*      ���ռ�ļ�.
			* @���� strValue
			*      ���ռ��ֵstring����.
			* @���� false:	��ʾ������key������ַ�������
			*	   true:    ���óɹ�
			*/
			virtual bool SetEx(const std::string& strKey, const std::string& strValue, unsigned int uTimeoutSeconds) = 0;

			/*
			* ����key���������ַ���ֵ
			* @���� strKey
			*      ���򼯼�ֵ.
			* @���� strValue
			*      ���ռ��ֵstring����.
			* @���� false:	��ʾ������key������ַ�������
			*	   true:    ���óɹ�
			*/
			virtual bool Get(const std::string& strKey, std::string& strValue) = 0;

			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*									   Redis��ϣ����hash�ӿ�									   */
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*
			* ����p_pcKey����Ӧ�Ĺ�ϣ��p_strField�ֶε�ֵΪp_strValue
			* @���� p_pcKey/p_strKey
			*      ��ϣ�����key.
			* @���� p_strField
			*      ��ϣ�����������.
			* @���� p_strValue
			*      ��ϣ�������ֵ.
			* @���� false:	����
			*	   true:    �ɹ�
			*/
			virtual bool HSet(const char* p_pcKey, const char* p_strField, const char* p_strValue) = 0;
			virtual bool HSet(const std::string& p_strKey, const std::string& p_strField, std::string& p_strValue) = 0;
			/*
			* �����"��-ֵ"������� KEY ��Ӧ�Ĺ�ϣ����
			* @���� p_strKey
			*      ��ϣ�����key.
			* @���� p_mapItem
			*      ��ϣ�������-ֵ��.
			* @���� false:	����
			*	   true:    �ɹ�
			*/
			virtual bool HMSet(const std::string& p_strKey, const std::map<std::string, std::string>& p_mapItem) = 0;
			/*
			* ��redis��ϣ���л�ȡĳ�� key �����ĳ�����ֵ
			* @���� p_strKey
			*      ��ϣ�����key.
			* @���� p_strField
			*      ��ϣ�����������.
			* @���� p_strValue
			*      ��ϣ�������ֵ.
			* @���� false:	����
			*	   true:    �ɹ�
			*/
			virtual bool HGet(const std::string& p_strKey, const std::string& p_strField, std::string& p_strValue) = 0;
			/*
			* ��redis��ϣ���л�ȡĳ��key������������ֶε�ֵ
			* @���� p_strKey
			*      ��ϣ�����key.
			* @���� p_mapItem
			*      �洢���ֶ���-ֵ��ѯ�����.
			* @���� false:	����
			*	   true:    �ɹ�
			*/
			virtual bool HGetAll(const std::string& p_strKey, std::map<std::string, std::string>& p_mapItem) = 0;

			virtual bool HGetAllEx(const std::string& p_strKey, std::map<std::string, std::string>& p_mapItem) = 0;

			/*
			* ��redis��ϣ����ɾ��ĳ��key�����ĳЩ���ֶ�
			* @���� p_strKey
			*      ��ϣ�����key.
			* @���� p_pcField
			*      ���ֶ�����.
			* @���� false:	����
			*	   true:    �ɹ�
			*/
			virtual bool HDel(const std::string& p_strKey, const std::string& p_pcField) = 0;
			/*
			* ��redis��ϣ����ɾ��ĳ��key�����ĳЩ���ֶ�
			* @���� p_strKey
			*      ��ϣ�����key.
			* @���� p_pcField
			*      ���ֶ�����,���һ���ֶα�����NULL
			* @���� false:	����
			*	   true:    �ɹ�
			*/
			virtual bool HDelFields(const std::string& p_strKey, const char* p_pcField, ...) = 0;
			/*
			* ��redis��ϣ����ɾ��ĳ��key�����ĳЩ���ֶ�
			* @���� p_strKey
			*      ��ϣ�����key.
			* @���� p_vecFields
			*      ���ֶ����Ƽ���.
			* @���� false:	����
			*	   true:    �ɹ�
			*/
			virtual bool HDelFields(const std::string& p_strKey, const std::vector<std::string>& p_vecFields) = 0;
			/*
			* ��redis��ϣ���м��ĳ��key�����ĳ�����ֶ��Ƿ����
			* @���� p_strKey
			*      ��ϣ�����key.
			* @���� p_pcField
			*      ���ֶ�����.
			* @���� false:	����������key����ǹ�ϣ���������ֶβ�����
			*	   true:    ����
			*/
			virtual bool HExists(const std::string& p_strKey, const std::string& p_pcField) = 0;
			/*
			* �Ե�����ʽ�鲼��ϣp_pcKey�еļ�ֵ��
			* @���� p_strKey
			*      ��ϣ�����key.
			* @���� p_iCursor
			*      �α�ֵ�����α������봫��0��֮���Ժ������ص��α����.
			* @���� p_mapOut
			*      �洢�õ��Ľ�������ڲ���׷�ӵķ�ʽ��ӽ�������ڵ��øú���ǰ������������.
			* @���� p_strPattern
			*      ƥ��ģʽ��nullptr��Ч
			* @���� p_uiCount
			*      �޶��Ľ�����������ǿ�ָ��ʱ��Ч.
			* @���� ��һ�α�λ��
			*      0:��������
			*      -1������
			*      >0 �α����һλ��
			*/
			virtual int HScan(const std::string& p_strKey, int p_iCursor, std::map<std::string, std::string>& p_mapOut, const std::string& p_strPattern = "", const unsigned int* p_uiCount = nullptr) = 0;
			
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*									   Redis�б����ӿ�										   */
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*
			* ����ָ���б�����Ԫ�ظ���
			* @���� p_strKey
			*      �б�����key
			* @���� ָ���б����ĳ��ȣ���Ԫ�ظ�����
			*	   -1: ��������  
			*/
			virtual int LLen(const std::string& p_strKey) = 0;
			/*
			* ��һ������ֵԪ�ز��뵽�б����ı�β
			* @���� p_strKey
			*      �б�����key
			* @���� p_vecValues
			*      Ԫ��ֵ����
			* @���� ������ǰ�б�����е�Ԫ�ظ���,���ü�������ʱ������µ��б���󼰶����е�Ԫ��
			*	   -1: ��ʾ�����ö�����б����
			*/
			virtual int RPush(const std::string& p_strKey, const std::vector<std::string>& p_vecValues) = 0;
			/*
			* ���ض�Ӧ���б�����У�ָ���±��Ԫ��
			* @���� p_strKey
			*      �б�����key
			* @���� p_uiIndex
			*      �±�ֵ
			* @���� p_strResult
			*      �洢���
			* @���� true���������ɹ�����ʱ��p_strResult���ݷǿ��������ȷ�����ָ���±��Ԫ�أ�
			*      ���p_strResult.empty()��ʾû�л��Ԫ�أ�����falseʱ��������ʧ��
			*/
			virtual bool LIndex(const std::string& p_strKey, unsigned int p_uiIndex, std::string& p_strResult) = 0;
			/*
			* �����б���ָ�������ڣ������䣩��Ԫ�أ�������ƫ����p_iStart��p_iEndָ�����±���ʼֵ��0��ʼ��-1��ʾ���һ���±�ֵ
			* @���� p_strKey
			*      �б�����key
			* @���� p_iStart
			*      ��ʼ�±�ֵ
			* @���� p_iEnd
			*      �����±�ֵ
			* @���� p_vesResult
			*      �洢�б������ָ�������Ԫ�ؼ���
			* @���� true �ɹ�
			*      false ��ʾ�����p_strKey���б����
			*/
			virtual bool LRange(const std::string& p_strKey, int p_iStart, int p_iEnd, std::vector<std::string>& p_vesResult) = 0;
			/*
			* ���б��±�Ϊidx��Ԫ�ص�ֵ����Ϊvalue����idx����������Χ�����һ�����б�(key������)����lsetʱ������һ������
			* @���� p_strKey
			*      �б�����key
			* @���� p_uiIndex
			*      �±�λ�ã���Ϊ��ֵʱ���β����ͷβ����λ���������˳��ʽ��
			*      �磺0 ��ʾͷ����һ��Ԫ�أ�-1 ��ʾβ����ʼ�ĵ�һ��Ԫ��
			* @���� p_strValue
			*      Ԫ����ֵ
			* @���� true �ɹ�
			*      false key���б�����key�����ڻ�idx������Χ
			*/
			virtual bool LSet(const std::string& p_strKey, unsigned int p_uiIndex, const std::string& p_strValue) = 0;
			/*
			* ����Ԫ��ֵ���б�������Ƴ�ָ��������Ԫ��
			* @���� p_strKey
			*      �б�����key
			* @���� p_iCount
			*      �Ƴ�Ԫ�ص��������ƣ�p_iCount�ĺ�������
			*      p_iCount > 0 : �ӱ�ͷ��ʼ���β�������Ƴ���p_strValue��ȵ�Ԫ�أ�����Ϊp_iCount
			*      p_iCount < 0 : �ӱ�β��ʼ���ͷ�������Ƴ���p_strValue��ȵ�Ԫ�أ�����Ϊp_iCount�ľ���ֵ
			*      p_iCount = 0 : �Ƴ�����������p_strValue��ȵ�ֵ
			* @���� p_strValue
			*      ָ����Ԫ��ֵ����Ҫ���б�����б����������ֵ�Ƚ�
			* @���� ���Ƴ��Ķ�������������ֵ�������£�
			*      -1�� ������ p_strKey ������б����
			*      0��  key�����ڻ��Ƴ���Ԫ�ظ���Ϊ 0
			*      >0�� ���ɹ��Ƴ���Ԫ������
			*/
			virtual int LRem(const std::string& p_strKey, int p_iCount, const std::string& p_strValue) = 0;

			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*									   Redis����set����ӿ�								       */
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*
			* ��һ������memberԪ�ؼ��뵽����p_strKey���У��Ѿ������ڼ��ϵ�memberԪ�ؽ�������;
			* @���� p_strKey
			*      ���϶����key
			* @���� p_vecMembers
			*      ���ϳ�Ա
			* @���� ����ӵ������е���Ԫ�ص������������������Ե�Ԫ��
			*      -1: ��ʾ�����p_strKey�Ǽ��϶���
			*/
			virtual int SAdd(const std::string& p_strKey, const std::vector<std::string>& p_vecMembers) = 0;
			/*
			* ��ü��϶����г�Ա������
			* @���� p_strKey
			*      ���϶����key
			* @���� ���ظü��϶����г�Ա��������������
			*      -1: �����Ǽ��϶���
			*      0�� ��Ա����Ϊ�ջ��key������
			*      > 0����Ա�����ǿ�
			*/
			virtual int SCard(const std::string& p_strKey) = 0;
			/*
			* ���ؼ����е����г�Ա
			* @���� p_strKey
			*      ���϶����key
			* @���� p_vecResult
			*      �洢�����
			* @���� ���������
			*      -1: �����Ǽ��϶���
			*/
			virtual int SMembers(const std::string& p_strKey, std::vector<std::string>& p_vecResult) = 0;
			/*
			* �Ƴ������е�һ������memberԪ�أ������ڵ�memberԪ�ػᱻ����
			* @���� p_strKey
			*      ���϶����key
			* @���� p_vecMembers
			*      ��Ҫ���Ƴ��ĳ�Ա�б�
			* @���� ���Ƴ��ĳ�ԱԪ�صĸ���
			*      -1: �������Ǽ��϶���
			*      0:  key�����ڻ��Ա������
			*/
			virtual int SRem(const std::string& p_strKey, const std::vector<std::string>& p_vecMembers) = 0;
			/*
			* �ж�Ԫ���Ƿ�Ϊ�����е�Ԫ��
			* @���� p_strKey
			*      ���϶����key
			* @���� p_strMember
			*      ����ֵ
			* @���� true  ��
			*      false ���ǻ��߳����Ǽ��϶���
			*/
			virtual bool SIsMember(const std::string& p_strKey, const std::string& p_strMember) = 0;
			/*
			* �����и������ϵĲ
			* @���� p_strDstKey
			*      �洢�����
			* @���� p_strFirstKey
			*      ����1����ļ�ֵ
			* @���� p_strSecondKey
			*      ����2����ļ�ֵ
			* @���� ���������
			*      -1 ��ʾ�������һ���Ǽ��϶����ǻ��߳����Ǽ��϶���
			*/
			virtual int SDiffStore(const std::string& p_strDstKey, const std::string& p_strFirstKey, const std::string& p_strSecondKey) = 0;
			/*
			* �����и������ϵĽ���
			* @���� p_strDstKey
			*      �洢�����
			* @���� p_strFirstKey
			*      ����1����ļ�ֵ
			* @���� p_strSecondKey
			*      ����2����ļ�ֵ
			* @���� ���������
			*      -1 ��ʾ�������һ���Ǽ��϶����ǻ��߳����Ǽ��϶���
			*/
			virtual int SInterStore(const std::string& p_strDstKey, const std::string& p_strFirstKey, const std::string& p_strSecondKey) = 0;
			/*
			* �����и������ϵĲ���
			* @���� p_strDstKey
			*      �洢�����
			* @���� p_strFirstKey
			*      ����1����ļ�ֵ
			* @���� p_strSecondKey
			*      ����2����ļ�ֵ
			* @���� ���������
			*      -1 ��ʾ�������һ���Ǽ��϶����ǻ��߳����Ǽ��϶���
			*/
			virtual int SUnionStore(const std::string& p_strDstKey, const std::string& p_strFirstKey, const std::string& p_strSecondKey) = 0;
			
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*									   Redis����zset�ӿ�									   */
			/////////////////////////////////////////////////////////////////////////////////////////////////
			/*
			* ��Ӷ�Ӧ key ������
			* @���� strKey
			*      ���򼯼�ֵ.
			* @���� vMembers
			*      "��ֵ-��Ա"����.
			* @���� 0:	��ʾһ��Ҳδ��ӣ�������Ϊ�ó�Ա�Ѿ�������������
			*	   -1:	��ʾ����� key ��������򼯶���
			*	   >0:	����ӵĳ�Ա����
			*/
			virtual int ZAdd(const std::string strKey, const std::vector<std::pair<const char*, double> >&vMembers) = 0;
			/*
			* �����Ӧ�������򼯵ĳ�Ա����
			* @���� strKey
			*      ���򼯼�ֵ..
			* @���� 0:	�ü�������
			*	   -1:	�����ü������ݶ�������Ч�����򼯶���
			*	   >0:	��ǰ��ֵ��Ӧ�����ݶ����еĳ�Ա����
			*/
			virtual int ZCard(const std::string strKey) = 0;
			/*
			* ��� key ��������ָ����ֵ����ĳ�Ա����
			* @���� strKey
			*      ���򼯼�ֵ.
			* @���� dMin
			*      ��С��ֵ.
			* @���� dMax
			*      ����ֵ.
			* @���� 0:	�ü���Ӧ�����򼯲����ڻ�� KEY ���򼯵Ķ�Ӧ��ֵ�����ԱΪ��
			*	   -1:	�����ü������ݶ�������Ч�����򼯶���
			*	   >0:	���������ĳ�Ա����
			*/
			virtual int ZCount(const std::string strKey, double dMin, double dMax) = 0;
			/*
			* �� key �������е�ĳ����Ա�ķ�ֵ��������inc
			* @���� strKey
			*      ���򼯼�ֵ.
			* @���� dInc
			*      ����ֵ.
			* @���� strMember
			*      �����г�Ա��.
			* @���� dResult
			*      �ǿ�ʱ�洢���ֵ.
			* @���� true:	�����ɹ�
			*	   false:	����ʧ��
			*/
			virtual bool ZIncrby(const std::string strKey, double dInc, const std::string strMember, double* pResult = NULL) = 0;

			virtual bool ZIncrby(const std::string strKey, double dInc, const std::string strMember, unsigned int sLen, double* pResult = NULL) = 0;
			/*
			* �� key �������л��ָ��λ������ĳ�Ա���б���Ա����ֵ������ʽ����
			* @���� strKey
			*      ���򼯼�ֵ.
			* @���� nStart
			*      ��ʼ�±�λ��.
			* @���� nStop
			*      �����±�λ�ã������ͬʱ����λ�ã�.
			* @���� vResult
			*      �ǿ�ʱ�洢���ֵ,�ڲ��ȵ���result.clear()������е�Ԫ��.
			* @���� 0:	��ʾ�����Ϊ�ջ� key ������
			*	   -1:  ��ʾ����� key ��������򼯶���
			*	   >0:  �����������
			*
			*ע�������±�λ�ã�0 ��ʾ��һ����Ա��1 ��ʾ�ڶ�����Ա��-1 ��ʾ���һ����Ա��-2 ��ʾ�����ڶ�����Ա���Դ�����
			*
			*�����ɹ������ͨ��������һ��ʽ�������
			*1���ڵ��÷����д���ǿյĴ洢�������ĵ�ַ
			*2�����෽��get_value���ָ���±��Ԫ������
			*3�����෽�� get_child ���ָ���±��Ԫ�ض���(redis_result��Ȼ����ͨ��
			*   redis_result::argv_to_string �������Ԫ������
			*	redis_result::argv_to_string �������Ԫ������
			*4�����෽�� get_result ����ȡ���ܽ�������� redis_result��Ȼ����ͨ��
			*	redis_result::get_child ���һ��Ԫ�ض���Ȼ����ͨ����ʽ 2 ��ָ���ķ�����ø�Ԫ�ص�����
			*5�����෽�� get_children ��ý��Ԫ�����������ͨ��
			*	redis_result �еķ���argv_to_string��ÿһ��Ԫ�ض����л��Ԫ������
			*/
			virtual int ZRange(const std::string strKey, int nStart, int nStop, std::vector<std::string>* vResult) = 0;
			/*
			* �� key �������л��ָ��λ������ĳ�Ա������ֵ�б���Ա����ֵ������ʽ����
			* @���� strKey
			*      ���򼯼�ֵ.
			* @���� nStart
			*      ��ʼ�±�λ��.
			* @���� nStop
			*      �����±�λ�ã������ͬʱ����λ�ã�.
			* @���� vOut
			*      �洢 "��Ա��-��ֵ��"��������ڲ��ȵ��� out.clear().
			* @���� 0:	��ʾ�����Ϊ�ջ� key ������
			*	   -1:  ��ʾ����� key ��������򼯶���
			*	   >0:  �����������
			*
			*ע�������±�λ�ã�0 ��ʾ��һ����Ա��1 ��ʾ�ڶ�����Ա��-1 ��ʾ���һ����Ա��-2 ��ʾ�����ڶ�����Ա���Դ�����
			*/
			virtual int ZRangeWithScores(std::string strKey, int nStart, int nStop, std::vector<std::pair<std::string, double>>& vOut) = 0;
			/*
			* ��������ɾ��ĳ����Ա
			* @���� strKey
			*      ���򼯼�ֵ.
			* @���� vMembers
			*      Ҫɾ���ĳ�Ա�б�.
			* @���� 0:	��ʾ�����򼯲����ڻ��Ա������
			*	   -1:  ��ʾ������ key �����򼯶���
			*	   >0:  ��ʾ�ɹ�ɾ���ĳ�Ա����
			*/
			virtual int ZRem(std::string strKey, const std::vector<std::string>& vMembers) = 0;
			/*
			* �� key �������л��ָ��λ������ĳ�Ա���б���Ա����ֵ�ݼ���ʽ����
			* @���� strKey
			*      ���򼯼�ֵ.
			* @���� nStart
			*      ��ʼ�±�λ��.
			* @���� nStop
			*      �����±�λ�ã������ͬʱ����λ�ã�.
			* @���� pResult
			*      �ǿ�ʱ�洢�����.
			* @���� 0:	��ʾ�����򼯲����ڻ��Ա������
			*	   -1:  ��ʾ������ key �����򼯶���
			*	   >0:  ��ʾ�ɹ�ɾ���ĳ�Ա����
			*
			* ע�������±�λ�ã�0 ��ʾ��һ����Ա��1 ��ʾ�ڶ�����Ա��-1 ��ʾ���һ����Ա��-2 ��ʾ�����ڶ�����Ա���Դ�����
			*/
			virtual int ZRevrange(std::string strKey, int nStart, int nStop, std::vector<std::string>* pResult) = 0;
			/*
			* �� key �������л��ָ��λ������ĳ�Ա������ֵ�б���Ա����ֵ�ݼ���ʽ����
			* @���� strKey
			*      ���򼯼�ֵ.
			* @���� nStart
			*      ��ʼ�±�λ��.
			* @���� nStop
			*      �����±�λ�ã������ͬʱ����λ�ã�.
			* @���� vOut
			*      �洢 "��Ա��-��ֵ��"��������ڲ��ȵ��� out.clear().
			* @���� 0:	��ʾ�����򼯲����ڻ��Ա������
			*	   -1:  ��ʾ������ key �����򼯶���
			*	   >0:  ��ʾ�ɹ�ɾ���ĳ�Ա����
			*
			* ע�������±�λ�ã�0 ��ʾ��һ����Ա��1 ��ʾ�ڶ�����Ա��-1 ��ʾ���һ����Ա��-2 ��ʾ�����ڶ�����Ա���Դ�����
			*/
			virtual int ZRevrangeWithScores(std::string strKey, int nStart, int nStop, std::vector<std::pair<std::string, double>>& vOut) = 0;



			/*
			*  ������,��������SetNxʵ��
			* @���� p_pcKey
			*	   �ַ�������� key
			* @���� p_strLockValue
			*	   ���ɹ���p_pcKey ��Ӧ�� value
			* @���� p_iLockTTL
			*	   ���ɹ�����������ʱ��
			* @���� p_iTimeOut
			*	   �������ĳ�ʱʱ�䣬��p_iTimeOut����δ���������ʧ��
			* * @���� p_bOperatorLock
			*	   �Ƿ������
			* @���� p_strError
			*	   ���صĴ�����ʾ��Ϣ
			* @���� false:	ʧ��
			*	   true:    �����ɹ�
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