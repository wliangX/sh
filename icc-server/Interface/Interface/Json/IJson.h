#pragma once
#include <string>
#include <IObject.h>

namespace ICC
{
	namespace JsonParser
	{
		//************************************
		// Class:     IJson
		// Qualifier: ����Json�ַ������ж��ַ����ĺ�����
		//************************************
		class IJson :
			public IObject
		{
		public:
			//************************************
			// Method:    LoadFile
			// FullName:  ACS::IJson::LoadFile
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: ����Json�ļ�
			// Parameter: std::string p_strFile
			//************************************
			virtual bool LoadFile(std::string p_strFile) = 0;

			//************************************
			// Method:    SaveAs
			// FullName:  ACS::IJson::SaveAs
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: ��Jsonд���ļ�
			// Parameter: std::string p_strFile
			//************************************
			virtual bool SaveAs(std::string p_strFile) = 0;

			//************************************
			// Method:    LoadJson
			// FullName:  ACS::IJson::LoadJson
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: ����Json�ַ������ж��ַ����ĺ�����
			// Parameter: std::string p_strJsonMessage
			//************************************
			virtual bool LoadJson(std::string p_strJson) = 0;

			//************************************
			// Method:    ToString
			// FullName:  ACS::IJson::ToString
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: ��Json����ת��ΪJson�ַ���
			//************************************
			virtual std::string ToString() = 0;

			//************************************
			// Method:    GetNodeValue
			// FullName:  ACS::IJson::GetNodeValue
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier:
			// Parameter: std::string p_strNodePath
			//            /header/cmd,  
			//            /body/data/0/guid (0����data��һ���������ͣ�ȡ��һ��Ԫ�����ֵ)
			// Parameter: std::string p_strDefault
			//************************************
			virtual std::string GetNodeValue(std::string p_strNodePath, std::string p_strDefault) = 0;

			//************************************
			// Method:    SetNodeValue
			// FullName:  ACS::IJson::SetNodeValue
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: std::string p_strNodePath
			//             /header/cmd
			//             /body/data/0/guid (0����data��һ���������ͣ����õ�һ��Ԫ�����ֵ)
			// Parameter: std::string p_strValue
			//************************************
			virtual void SetNodeValue(std::string p_strNodePath, std::string p_strValue) = 0;
			virtual void SetNodeValue(std::string p_strNodePath, int p_nValue) = 0;
			//************************************
			// Method:    GetCount
			// FullName:  ACS::IJson::GetCount
			// Access:    virtual public 
			// Returns:   int
			// Qualifier: ��ȡ����ڵ�ĸ�����������ڵ㷵��-1���ڵ㲻���ڷ���-2
			// Parameter: std::string p_strNodePath
			//************************************
			virtual int GetCount(std::string p_strNodePath) = 0;

			//************************************
			// Method:    Find
			// FullName:  ACS::IJson::Find
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: ���Key�ڵ��Ƿ����
			// Parameter: std::string p_strNodePath
			//************************************
			virtual bool Find(std::string p_strNodePath) = 0;
		};

		typedef boost::shared_ptr<IJson> IJsonPtr;
	}
}
