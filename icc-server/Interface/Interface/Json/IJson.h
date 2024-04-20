#pragma once
#include <string>
#include <IObject.h>

namespace ICC
{
	namespace JsonParser
	{
		//************************************
		// Class:     IJson
		// Qualifier: 解析Json字符串，判断字符串的合理性
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
			// Qualifier: 加载Json文件
			// Parameter: std::string p_strFile
			//************************************
			virtual bool LoadFile(std::string p_strFile) = 0;

			//************************************
			// Method:    SaveAs
			// FullName:  ACS::IJson::SaveAs
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: 将Json写入文件
			// Parameter: std::string p_strFile
			//************************************
			virtual bool SaveAs(std::string p_strFile) = 0;

			//************************************
			// Method:    LoadJson
			// FullName:  ACS::IJson::LoadJson
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: 解析Json字符串，判断字符串的合理性
			// Parameter: std::string p_strJsonMessage
			//************************************
			virtual bool LoadJson(std::string p_strJson) = 0;

			//************************************
			// Method:    ToString
			// FullName:  ACS::IJson::ToString
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: 将Json对象转换为Json字符串
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
			//            /body/data/0/guid (0代表data是一个数组类型，取第一个元素里的值)
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
			//             /body/data/0/guid (0代表data是一个数组类型，设置第一个元素里的值)
			// Parameter: std::string p_strValue
			//************************************
			virtual void SetNodeValue(std::string p_strNodePath, std::string p_strValue) = 0;
			virtual void SetNodeValue(std::string p_strNodePath, int p_nValue) = 0;
			//************************************
			// Method:    GetCount
			// FullName:  ACS::IJson::GetCount
			// Access:    virtual public 
			// Returns:   int
			// Qualifier: 获取数组节点的个数，非数组节点返回-1，节点不存在返回-2
			// Parameter: std::string p_strNodePath
			//************************************
			virtual int GetCount(std::string p_strNodePath) = 0;

			//************************************
			// Method:    Find
			// FullName:  ACS::IJson::Find
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: 检测Key节点是否存在
			// Parameter: std::string p_strNodePath
			//************************************
			virtual bool Find(std::string p_strNodePath) = 0;
		};

		typedef boost::shared_ptr<IJson> IJsonPtr;
	}
}
