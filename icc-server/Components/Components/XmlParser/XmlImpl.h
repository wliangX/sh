#pragma once

#include <list>
#include <string>
#include "tinyxml2.h"
using namespace tinyxml2;

namespace ICC
{
	namespace Xml
	{
		class CXmlImpl :
			public IXml
		{
		public:
			CXmlImpl();
			~CXmlImpl();

		public:
			virtual bool LoadFile(std::string p_strFile);
			virtual bool SaveAs(std::string p_strFile);

			virtual bool LoadXml(std::string p_strXml);
			virtual std::string GetXml();

			virtual void SetText(std::string p_strNodePath, std::string p_strValue);
			virtual std::string GetText(std::string p_strNodePath, std::string p_strDefault);

			virtual void SetAttr(std::string p_strNodePath, std::string p_strAttr, std::string p_strValue);
			virtual std::string GetAttr(std::string p_strNodePath, std::string p_strAttr, std::string p_strDefault);

			virtual unsigned int GetChildCount(std::string p_strNodePath, std::string p_strChild);
			virtual bool Find(std::string p_strNodePath);

		private:
			std::list<std::string> SplitNodePath(std::string p_strNodePath);
			XMLNode* GetNode(std::string p_strNodePath, bool p_bAutoCreate);

		private:
			XMLDocument* m_pDoc;
		};
	}
}
