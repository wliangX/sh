#include "Boost.h"
#include "ResultSetImpl.h"

CResultSetImpl::CResultSetImpl(PGResultPtr p_pPGResult)
	:m_pResult(p_pPGResult),m_uiRecordSize(0),m_uiCurRow(-1),m_bValid(false)
{
	
}

CResultSetImpl::~CResultSetImpl()
{

}

void CResultSetImpl::SetRecordSize(unsigned int p_uiSize)
{
	m_uiRecordSize = p_uiSize;
}

bool CResultSetImpl::IsValid()
{
	return m_bValid;
}

bool CResultSetImpl::Next()
{
	int i = m_uiRecordSize - 1;
	if (m_uiRecordSize > 0 && m_uiCurRow < i)
	{		
		++m_uiCurRow;
		return true;
	}
	return false;
}

unsigned int CResultSetImpl::FieldSize()
{
	return m_mapFieldIndex.size();
}

unsigned int CResultSetImpl::RecordSize()
{
	return m_uiRecordSize;
}

unsigned int CResultSetImpl::GetCurRow()
{
	return m_uiCurRow;
}

bool CResultSetImpl::SetCurRow(unsigned int p_uiCurRow)
{
	if (p_uiCurRow>=m_uiRecordSize)
	{
		return false;
	}
	m_uiCurRow = p_uiCurRow;
	return true;
}

std::vector<std::string> CResultSetImpl::GetFieldNames()
{
	return m_vecFieldNames;
}

std::string CResultSetImpl::GetValue(unsigned int p_uiRow, unsigned int p_uiCol, const std::string p_strDefault /*= ""*/)
{
	const char* l_temp = PQgetvalue(m_pResult.get(), p_uiRow, p_uiCol);
	std::string l_strResult = l_temp ? l_temp : "";
	return l_strResult.empty() ? p_strDefault : l_strResult;
}

std::string CResultSetImpl::GetValue(unsigned int p_uiRow, const std::string p_strFieldName, const std::string p_strDefault/* = "default"*/)
{
	//如果传入一个错误的列名。。。
	auto it = m_mapFieldIndex.find(p_strFieldName);
#ifdef _DEBUG
	return GetValue(p_uiRow, it->second, p_strDefault);
#else
	if (it != m_mapFieldIndex.end())
	{
		return GetValue(p_uiRow, it->second, p_strDefault);
	}
	return p_strDefault;
#endif
}

std::string CResultSetImpl::GetValue(const std::string p_strFieldName, const std::string p_strDefault /*= ""*/)
{
	return GetValue(m_uiCurRow, p_strFieldName,p_strDefault);
}

std::string CResultSetImpl::GetErrorMsg()
{
	return m_strErrorMsg;
}

void CResultSetImpl::SetErrorMsg(std::string p_strMsg)
{
	m_strErrorMsg = p_strMsg;
}

void CResultSetImpl::SetResultStatus(bool p_bIsValid)
{
	m_bValid = p_bIsValid;
}

void CResultSetImpl::AddFieldIndex(std::string p_strFieldName, unsigned int p_uiFieldIndex)
{
	m_vecFieldNames.push_back(p_strFieldName);
	m_mapFieldIndex[p_strFieldName] = p_uiFieldIndex;
}

void CResultSetImpl::SetSQL(std::string p_strSQL)
{
	m_strSQL = p_strSQL;
}

std::string CResultSetImpl::GetSQL()
{
	return m_strSQL;
}
