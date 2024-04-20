#pragma once
#include <string>
#include <IObject.h>

// Root/Node[0]/Node[1]/Node[2]
namespace ICC
{
	namespace Xml
	{
		class IXml :
			public IObject
		{
		public:
			virtual bool LoadFile(std::string p_strFile) = 0;
			virtual bool SaveAs(std::string p_strFile) = 0;

			virtual bool LoadXml(std::string p_strXml) = 0;
			virtual std::string GetXml() = 0;

			virtual void SetText(std::string p_strNodePath, std::string p_strValue) = 0;
			virtual std::string GetText(std::string p_strNodePath, std::string p_strDefault) = 0;

			virtual void SetAttr(std::string p_strNodePath, std::string p_strAttr, std::string p_strValue) = 0;
			virtual std::string GetAttr(std::string p_strNodePath, std::string p_strAttr, std::string p_strDefault) = 0;

			virtual unsigned int GetChildCount(std::string p_strNodePath, std::string p_strChild) = 0;
			virtual bool Find(std::string p_strNodePath) = 0;
		};

		typedef boost::shared_ptr<IXml> IXmlPtr;
	}
}
