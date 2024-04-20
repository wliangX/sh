#include "Boost.h"
#include "XmlImpl.h"

CXmlImpl::CXmlImpl()
{
	m_pDoc = new XMLDocument;
}

CXmlImpl::~CXmlImpl()
{
	delete m_pDoc;
}

bool CXmlImpl::LoadFile(std::string p_strFile)
{
	m_pDoc->LoadFile(p_strFile.c_str());
	return m_pDoc->ErrorID() == XML_SUCCESS;
}

bool CXmlImpl::SaveAs(std::string p_strFile)
{
	m_pDoc->SaveFile(p_strFile.c_str());
	return m_pDoc->ErrorID() == XML_SUCCESS;
}

bool CXmlImpl::LoadXml(std::string p_strXml)
{
	m_pDoc->Parse(p_strXml.c_str());
	return m_pDoc->ErrorID() == XML_SUCCESS;
}

std::string CXmlImpl::GetXml()
{
	XMLPrinter prt;
	m_pDoc->Print(&prt);
	return prt.CStr();
}

void CXmlImpl::SetText(std::string p_strNodePath, std::string p_strValue)
{
	XMLNode* l_pNode = GetNode(p_strNodePath, true);
	if (l_pNode && l_pNode->ToElement())
	{
		l_pNode->ToElement()->SetText(p_strValue.c_str());
	}
}

std::string CXmlImpl::GetText(std::string p_strNodePath, std::string p_strDefault)
{	
	XMLNode* l_pNode = GetNode(p_strNodePath, false);
	if (l_pNode && l_pNode->ToElement())
	{
		if (l_pNode->ToElement()->GetText())
		{
			return l_pNode->ToElement()->GetText();
		} 
		return "";		
	}
	else
	{
		printf("not find node! nodepath=%s\n", p_strNodePath.c_str());
	}
	return p_strDefault;
}

void CXmlImpl::SetAttr(std::string p_strNodePath, std::string p_strAttr, std::string p_strValue)
{
	XMLNode* l_pNode = GetNode(p_strNodePath, true);
	if (l_pNode && l_pNode->ToElement())
	{
		l_pNode->ToElement()->SetAttribute(p_strAttr.c_str(), p_strValue.c_str());
	}
}

std::string CXmlImpl::GetAttr(std::string p_strNodePath, std::string p_strAttr, std::string p_strDefault)
{
	XMLNode* l_pNode = GetNode(p_strNodePath, false);
	if (l_pNode && l_pNode->ToElement())
	{
		if (l_pNode->ToElement()->Attribute(p_strAttr.c_str()))
		{
			return l_pNode->ToElement()->Attribute(p_strAttr.c_str());
		}
		return "";
	}
	return p_strDefault;
}

unsigned int CXmlImpl::GetChildCount(std::string p_strNodePath, std::string p_strChild)
{
	unsigned int l_uiCount = 0;
	XMLNode* l_pNode = GetNode(p_strNodePath, false);
	if (l_pNode)
	{
		XMLNode* l_pChildNode = l_pNode->FirstChildElement(p_strChild.c_str());
		while (l_pChildNode)
		{
			l_pChildNode = l_pChildNode->NextSiblingElement(p_strChild.c_str());
			l_uiCount++;
		}
	}
	return l_uiCount;
}

bool CXmlImpl::Find(std::string p_strNodePath)
{
	return GetNode(p_strNodePath, false) ? true : false;
}

std::list<std::string> CXmlImpl::SplitNodePath(std::string p_strNodePath)
{
	std::list<std::string> l_lstNodeName;
	size_t l_nBegin = 0;
	size_t l_nFind = p_strNodePath.find('/');
	while (std::string::npos != l_nFind)
	{
		if (l_nBegin < l_nFind)
		{
			l_lstNodeName.push_back(p_strNodePath.substr(l_nBegin, l_nFind - l_nBegin));
		}

		l_nBegin = l_nFind + 1;
		l_nFind = p_strNodePath.find('/', l_nBegin);
	}
	if (l_nBegin < p_strNodePath.length())
	{
		l_lstNodeName.push_back(p_strNodePath.substr(l_nBegin));
	}
	return l_lstNodeName;
}

XMLNode* CXmlImpl::GetNode(std::string p_strNodePath, bool p_bAutoCreate)
{
	std::list<std::string> l_lstNodeName = SplitNodePath(p_strNodePath);
	if (l_lstNodeName.empty())
		return 0;
	

	XMLNode* l_pNode = m_pDoc;
	if (!l_pNode)
		return 0;

	for (std::string l_strNodeName : l_lstNodeName)
	{
		int l_nIndex = 0;
		size_t l_nLeft = l_strNodeName.find('[');
		size_t l_nRight = l_strNodeName.find(']');
		if (std::string::npos != l_nLeft && std::string::npos != l_nRight && l_nLeft + 1 < l_nRight)
		{
			l_nIndex = atoi(l_strNodeName.substr(l_nLeft + 1, l_nRight - l_nLeft - 1).c_str());
			l_strNodeName = l_strNodeName.substr(0, l_nLeft);
		}

		XMLNode* l_pChildNode = l_pNode->FirstChildElement(l_strNodeName.c_str());
		if (!l_pChildNode && p_bAutoCreate)
			l_pChildNode = l_pNode->LinkEndChild(m_pDoc->NewElement(l_strNodeName.c_str()));
		if (!l_pChildNode)
			return 0;

		for (int i = 0; i < l_nIndex; i++)
		{
			l_pChildNode = l_pChildNode->NextSiblingElement(l_strNodeName.c_str());
			if (!l_pChildNode && p_bAutoCreate)
				l_pChildNode = l_pNode->LinkEndChild(m_pDoc->NewElement(l_strNodeName.c_str()));
			if (!l_pChildNode)
				return 0;
		}

		l_pNode = l_pChildNode;
	}

	return l_pNode;
}