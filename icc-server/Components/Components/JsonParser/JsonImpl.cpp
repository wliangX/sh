#include "Boost.h"
#include "JsonImpl.h"

CJsonImpl::CJsonImpl(IResourceManagerPtr pResourceManager)
{
	m_pResourceManager = pResourceManager;
	m_pStrUti = ICCGetIStringFactory()->CreateString();
}

CJsonImpl::~CJsonImpl()
{
}

IResourceManagerPtr CJsonImpl::GetResourceManager()
{
	return m_pResourceManager;
}

bool CJsonImpl::LoadJson(std::string p_strJson)
{
	/*bool l_bRes = m_oDoc.Parse(p_strJson.c_str()).HasParseError();
	return !l_bRes;*/

	m_oDoc.Parse(p_strJson.c_str());
	if (!m_oDoc.IsObject() || m_oDoc.HasParseError())
	{
		return false;
	}

	return true;
}

bool CJsonImpl::LoadFile(std::string p_strFile)
{

	if (p_strFile.empty())
	{
		return false;
	}
	try
	{
		std::ifstream l_ifsStream(p_strFile.c_str(), std::ios_base::out);
		if (!l_ifsStream.is_open())
		{
			return false;
		}
		rapidjson::IStreamWrapper l_iSW(l_ifsStream);
		rapidjson::Writer<rapidjson::IStreamWrapper>l_oWriter(l_iSW);

		m_oDoc.ParseStream(l_iSW);
		l_ifsStream.close();
	}
	catch (...)
	{
		std::cerr << "unexpected error" << std::endl;
	}
	return true;
}

bool CJsonImpl::SaveAs(std::string p_strFile)
{
	if (p_strFile.empty())
	{
		return false;
	}
	try
	{
		std::ofstream l_ofsStream(p_strFile.c_str(), std::ios_base::app);
		if (!l_ofsStream.is_open())
		{
			return false;
		}
		rapidjson::OStreamWrapper l_oSW(l_ofsStream);
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper>l_oWriter(l_oSW);
		m_oDoc.Accept(l_oWriter);

		l_ofsStream.close();
	}
	catch (...)
	{
		std::cerr << "unexpected error" << std::endl;
	}
	return true;
}

std::string CJsonImpl::ToString()
{
	rapidjson::StringBuffer l_oBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> l_oWriter(l_oBuffer);
	m_oDoc.Accept(l_oWriter);
	std::string l_strResult = l_oBuffer.GetString();

	return l_strResult;
}

std::string CJsonImpl::GetNodeValue(std::string p_strNodePath, std::string p_strDefault)
{
	rapidjson::Pointer l_oPointer(p_strNodePath.c_str());
	if (!l_oPointer.IsValid())
	{
		return p_strDefault;
	}

	const rapidjson::Value* l_poValue = l_oPointer.Get(m_oDoc, 0);
	if (!l_poValue)
	{//节点不存在返回p
		return p_strDefault;
	}
	else if (l_poValue->IsNull())
	{
		return "";
	}
	else if (l_poValue->IsString())
	{
		return l_poValue->GetString();
	}
    else if (l_poValue->IsInt())
    {
		return m_pStrUti->Number(l_poValue->GetInt());
    }
    else if (l_poValue->IsFloat())
    {
		return  m_pStrUti->Number(l_poValue->GetFloat());
    }
    else if (l_poValue->IsDouble())
    {
		return  m_pStrUti->Number(l_poValue->GetDouble());
    }
    else if (l_poValue->IsBool())
    {
		return  m_pStrUti->Number(l_poValue->GetBool());
    }
	else if (l_poValue->IsObject())
	{
		rapidjson::StringBuffer l_oBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> l_oWriter(l_oBuffer);
		l_poValue->Accept(l_oWriter);
		return l_oBuffer.GetString();
	}
	else if (l_poValue->IsArray())
	{
		rapidjson::StringBuffer l_oBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> l_oWriter(l_oBuffer);
		l_poValue->Accept(l_oWriter);
		return l_oBuffer.GetString();
	}
	else
	{
		return p_strDefault;
	}
}

void CJsonImpl::SetNodeValue(std::string p_strNodePath, std::string p_strValue)
{
	rapidjson::Pointer l_oPointer(p_strNodePath.c_str());
	if (!l_oPointer.IsValid())
	{
		return;
	}

	l_oPointer.Set(m_oDoc, p_strValue.c_str());
}

void CJsonImpl::SetNodeValue(std::string p_strNodePath, int p_nValue)
{
	rapidjson::Pointer l_oPointer(p_strNodePath.c_str());
	if (!l_oPointer.IsValid())
	{
		return;
	}

	l_oPointer.Set(m_oDoc, p_nValue);
}

int CJsonImpl::GetCount(std::string p_strNodePath)
{
	rapidjson::Pointer l_oPointer(p_strNodePath.c_str());
	if (!l_oPointer.IsValid())
	{
		return -2;
	}

	const rapidjson::Value* l_poValue = l_oPointer.Get(m_oDoc, 0);
	if (!l_poValue)
	{
		return -2;
	}

	if (l_poValue->IsArray())
	{
		return l_poValue->Size();
	}
	else
	{
		return -1;
	}
}

bool CJsonImpl::Find(std::string p_strNodePath)
{
	rapidjson::Pointer l_oPointer(p_strNodePath.c_str());
	if (!l_oPointer.IsValid())
	{
		return false;
	}

	const rapidjson::Value* l_poValue = l_oPointer.Get(m_oDoc, 0);
	if (!l_poValue)
	{
		return false;
	}
	return true;
}

