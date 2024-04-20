#pragma once
//STL
#include <string>
#include <vector>

#include <boost/cstdint.hpp>

//Project
#include <IResource.h>
#include <TypeDef.h>


namespace ICC
{
	using boost::int16_t;
	using boost::uint16_t;
	using boost::int32_t;
	using boost::uint32_t;
	using boost::int64_t;
	using boost::uint64_t;

	namespace StringUtil
	{
		/*
		* class   �ַ����ӿ���
		* author  w16314
		* purpose
		* note
		*/
		class IStringUtil : public IResource
		{
		public:

			//************************************
			// Method:    AnsiToUtf8
			// FullName:  ACS::StringUtil::IStringUtil::AnsiToUtf8
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: Ansi����ת��ΪUtf8����
			// Parameter: std::string p_strAnsi
			//************************************
			virtual std::string AnsiToUtf8(std::string p_strAnsi) = 0;
			//************************************
			// Method:    Utf8ToAnsi
			// FullName:  ACS::StringUtil::IStringUtil::Utf8ToAnsi
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: Utf8����ת��ΪAnsi����
			// Parameter: std::string p_strUtf8
			//************************************
			virtual std::string Utf8ToAnsi(std::string p_strUtf8) = 0;

			//************************************
			// Method:    CreateGuid
			// FullName:  ACS::StringUtil::IStringUtil::CreateGuid
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: ����GUID
			//************************************
			virtual std::string CreateGuid() = 0;

			//************************************
			// Method:    CreateRandom
			// FullName:  ACS::StringUtil::IStringUtil::CreateRandom
			// Access:    virtual public 
			// Returns:   int32_t
			// Qualifier: ��p_iMin��p_iMax���������
			// Parameter: int32_t p_iMin
			// Parameter: int32_t p_iMax
			//************************************
			virtual int32_t CreateRandom(int32_t p_iMin = 0, int32_t p_iMax = 9) = 0;

			//************************************
			// Method:    CreateSerial
			// FullName:  ACS::StringUtil::IStringUtil::CreateSerial
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: ���̶�����������ˮ����
			// Parameter: int32_t p_iLen
			//************************************
			virtual std::string CreateSerial(int32_t p_iLen = 1) = 0;

			//************************************
			// Method:    Format
			// FullName:  ACS::StringUtil::IStringUtil::Format
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: �ַ�����ʽ��
			// Parameter: const char * p_pformat
			// Parameter: ...
			//************************************
			virtual std::string Format(const char* p_pformat, ...) = 0;

			//************************************
			// Method:    Find
			// FullName:  ACS::StringUtil::IStringUtil::Find
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: ���ַ�������p_strFind�Ƿ���ڣ����ڷ���true�������ڷ��� false��
			// Parameter: std::string p_str
			// Parameter: std::string p_strFind
			//************************************
			virtual bool Find(std::string p_str, std::string p_strFind) = 0;

			//************************************
			// Method:    Find
			// FullName:  ACS::StringUtil::IStringUtil::FindIndex
			// Access:    virtual public 
			// Returns:   ACS::int32_t
			// Qualifier: ��p_iIndex��ʼ���ҵ��ַ���p_strFind��n�γ��ֵ�λ�ã�n��0��ʼ�����ڷ���λ�ã������ڷ���-1
			// Parameter: std::string p_str
			// Parameter: std::string p_str
			// Parameter: std::string p_strFind
			// Parameter: uint32_t p_iIndex
			//************************************
			virtual int32_t FindIndex(std::string p_str, std::string p_strFind, uint32_t p_iIndex) = 0;

			//************************************
			// Method:    FindFirst
			// FullName:  ACS::StringUtil::IStringUtil::FindFirst
			// Access:    virtual public 
			// Returns:   int32_t
			// Qualifier: �ҵ��ַ���p_strFind��һ�γ��ֵ�λ��, ����p_strFind��һ�γ��ֵ�λ�ã����p_strFindû�б��ҵ������� -1
			// Parameter: std::string p_str
			// Parameter: std::string p_strFind
			//************************************
			virtual int32_t FindFirst(std::string p_str, std::string p_strFind) = 0;

			//************************************
			// Method:    FindLast
			// FullName:  ACS::StringUtil::IStringUtil::FindLast
			// Access:    virtual public 
			// Returns:   int32_t
			// Qualifier: �ҵ��ַ���p_strFind���һ�γ��ֵ�λ��, ����p_strFind���һ�γ��ֵ�λ�ã����p_strFindû�б��ҵ������� -1
			// Parameter: std::string p_str 
			// Parameter: std::string p_strFind
			//************************************
			virtual int32_t FindLast(std::string p_str, std::string p_strFind) = 0;

			//************************************
			// Method:    Contains
			// FullName:  ACS::StringUtil::IStringUtil::Contains
			// Access:    virtual public 
			// Returns:   int32_t
			// Qualifier: ����������ַ�����p_strFind���ֵĴ���
			// Parameter: std::string p_str
			// Parameter: std::string p_strFind
			//************************************
			virtual int32_t Contains(std::string p_str, std::string p_strFind) = 0;

			//************************************
			// Method:    RegexMatch
			// FullName:  ACS::StringUtil::IStringUtil::RegexMatch
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: ������ʽ����
			// Parameter: std::string p_str
			// Parameter: std::string p_strRegex
			//************************************
			virtual bool RegexMatch(std::string p_str, std::string p_strRegex) = 0;

			//************************************
			// Method:    RegexMatch
			// FullName:  ACS::StringUtil::IStringUtil::RegexMatch
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: ������ʽ����
			// Parameter: std::string p_str
			// Parameter: std::string p_strRegex
			// Parameter: std::vector<std::string> & p_vecWhat
			//************************************
			virtual bool RegexMatch(std::string p_str, std::string p_strRegex, std::vector<std::string>& p_vecWhat) = 0;

			//************************************
			// Method:    Split
			// FullName:  ACS::StringUtil::IStringUtil::Split
			// Access:    virtual public 
			// Returns:   int32_t
			// Qualifier: �ַ����ָ� p_bMultiSymbols �ָ����֧��ͬʱ�������ƥ�䣬
			// p_bMultiSymbols = true , ֧�� ����",./;!" ƥ��������Ž��зָ 
			// p_bMultiSymbols = false , p_strSplit ��Ϊһ��������зָ�
			// Parameter: std::string p_str
			// Parameter: std::string p_strSplit
			// Parameter: std::vector<std::string> & p_SplitVec
			// Parameter: bool p_bMultiSymbols
			//************************************
			virtual int32_t Split(std::string p_str, std::string p_strSplit, std::vector<std::string>& p_SplitVec, bool p_bMultiSymbols) = 0;

			//************************************
			// Method:    Left
			// FullName:  ACS::StringUtil::IStringUtil::Left
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: ���ذ����ַ����������len���ַ������ַ���, ���len�����ַ����ĳ��ȣ��������ַ���������
			// Parameter: std::string p_str
			// Parameter: int32_t p_iLen
			//************************************
			virtual std::string Left(std::string p_str, int32_t p_iLen) = 0;

			//************************************
			// Method:    Right
			// FullName:  ACS::StringUtil::IStringUtil::Right
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: ���ذ����ַ����������len���ַ������ַ���, ���len�����ַ����ĳ��ȣ��������ַ���������
			// Parameter: std::string p_str
			// Parameter: int32_t p_iLen
			//************************************
			virtual std::string Right(std::string p_str, int32_t p_iLen) = 0;


			//************************************
			// Method:    Mid
			// FullName:  ACS::StringUtil::IStringUtil::Mid
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: ���ذ�������ַ�����p_iIndexλ�ÿ�ʼp_iLen���ַ����ַ�����
			// �������ַ���Ϊ�ջ���index������Χ�����ؿ��ַ��������p_iIndex + p_iLen��������ַ����ĳ��ȣ����������ַ�����
			// Parameter: std::string p_str
			// Parameter: uint32_t p_iIndex
			// Parameter: int32_t p_iLen
			//************************************
			virtual std::string Mid(std::string p_str, uint32_t p_iIndex, int32_t p_iLen) = 0;

			//************************************
			// Method:    StartsWith
			// FullName:  ACS::StringUtil::IStringUtil::StartsWith
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: ����ַ�����s��ʼ�������棬���򷵻ؼ�
			// Parameter: std::string p_str
			// Parameter: std::string p_strTag
			//************************************
			virtual bool StartsWith(std::string p_str, std::string p_strTag) = 0;

			//************************************
			// Method:    EndsWith
			// FullName:  ACS::StringUtil::IStringUtil::EndsWith
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: ����ַ�����s�����������棬���򷵻ؼ١�
			// Parameter: std::string p_str
			// Parameter: std::string p_strTag
			//************************************
			virtual bool EndsWith(std::string p_str, std::string p_strTag) = 0;

			//************************************
			// Method:    Lower
			// FullName:  ACS::StringUtil::IStringUtil::Lower
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: ����������ַ���ת����Сд�ַ���
			// Parameter: std::string p_str
			//************************************
			virtual std::string Lower(std::string p_str) = 0;

			//************************************
			// Method:    Upper
			// FullName:  ACS::StringUtil::IStringUtil::Upper
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: ����������ַ���ת���Ĵ�д�ַ���
			// Parameter: std::string p_str
			//************************************
			virtual std::string Upper(std::string p_str) = 0;

			//************************************
			// Method:    ReplaceAll
			// FullName:  ACS::StringUtil::IStringUtil::ReplaceAll
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: ���ַ�����ʹ��p_strReplaceȫ���滻p_strSearch
			// Parameter: std::string p_str
			// Parameter: std::string p_strSearch
			// Parameter: std::string p_strReplace
			//************************************
			virtual std::string ReplaceAll(std::string p_str, std::string p_strSearch, std::string p_strReplace) = 0;

			//************************************
			// Method:    ReplaceFirst
			// FullName:  ACS::StringUtil::IStringUtil::ReplaceFirst
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: ���ַ�����ʹ��p_strReplace�滻��һ�γ��ֵ�p_strSearch
			// Parameter: std::string p_str
			// Parameter: std::string p_strSearch
			// Parameter: std::string p_strReplace
			//************************************
			virtual std::string ReplaceFirst(std::string p_str, std::string p_strSearch, std::string p_strReplace) = 0;

			//************************************
			// Method:    ReplaceLast
			// FullName:  ACS::StringUtil::IStringUtil::ReplaceLast
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: ���ַ�����ʹ��p_strReplace�滻���һ�γ��ֵ�p_strSearch
			// Parameter: std::string p_str
			// Parameter: std::string p_strSearch
			// Parameter: std::string p_strReplace
			//************************************
			virtual std::string ReplaceLast(std::string p_str, std::string p_strSearch, std::string p_strReplace) = 0;

			//************************************
			// Method:    Replace
			// FullName:  ACS::StringUtil::IStringUtil::Replace
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: ���ַ����е�p_iForwardIndexλ�ÿ�ʼ��p_iReverseIndex���滻Ϊp_strReplace
			//p_iForwardIndex���ַ������0��ʼ������p_iReverseIndex���ַ����Ҳ�0��ʼ����
			//��������ַ����ĳ���,����ԭ״��
			// Parameter: std::string p_str
			// Parameter: std::string p_strReplace
			// Parameter: uint32_t p_iForwardIndex
			// Parameter: uint32_t p_iReverseIndex
			//************************************
			virtual std::string Replace(std::string p_str, std::string p_strReplace, uint32_t p_iForwardIndex, uint32_t p_iReverseIndex) = 0;

			/************************************************************************/
			/* �ַ���ת���֣�֧�ֲ�ͬ����ת��                                          */
			/************************************************************************/
			virtual boost::int16_t ToShort(std::string p_str) = 0;
			virtual boost::uint16_t ToUShort(std::string p_str) = 0;
			virtual boost::int32_t ToInt(std::string p_str) = 0;
			virtual boost::uint32_t ToUInt(std::string p_str) = 0;
			virtual boost::int64_t ToInt64(std::string p_str) = 0;
			virtual boost::uint64_t ToUInt64(std::string p_str) = 0;
			virtual float ToFloat(std::string p_str) = 0;
			virtual double ToDouble(std::string p_str) = 0;

			/************************************************************************/
			/* ����ת�ַ���                                                         */
			/************************************************************************/
			virtual std::string Number(boost::int16_t n) = 0;
			virtual std::string Number(boost::uint16_t n) = 0;
			virtual std::string Number(boost::int32_t n) = 0;
			virtual std::string Number(boost::uint32_t n) = 0;
			virtual std::string Number(boost::int64_t n) = 0;
			virtual std::string Number(boost::uint64_t n) = 0;
			virtual std::string Number(float n) = 0;
			virtual std::string Number(double n) = 0;
			virtual std::string Number(bool n) = 0;

			//ȥ�ո�
			virtual std::string Trim(std::string p_str) = 0;
			virtual std::string TrimLeft(std::string p_str) = 0;
			virtual std::string TrimRight(std::string p_str) = 0;

			//�ϲ��ַ���
			//strSplit ���ӷ�
			virtual std::string Join(std::vector<std::string>& p_vecStr, std::string p_strSplit) = 0;
			virtual std::string Join(std::list<std::string>& p_lstStr, std::string p_strSplit) = 0;

		};

		typedef boost::shared_ptr<IStringUtil> IStringUtilPtr;
	}
}
