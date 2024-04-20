#include "Boost.h"
#include "ResultSet.h"
#include "ResultSetCounter.h"

CResultSet::CResultSet()
	:m_pPool(nullptr),m_pConnection(nullptr),m_pStmt(nullptr), m_pResultSet(nullptr),m_uiRecordSize(0),m_uiCurRow(-1),m_bValid(false)
{
	CResultSetCounter::Instance()->AddCounter();
}

CResultSet::~CResultSet()
{
	try
	{
		if (m_pStmt && m_pResultSet)
		{
			m_pStmt->closeResultSet(m_pResultSet);
		}

		if (m_pConnection && m_pStmt)
		{
			m_pConnection->terminateStatement(m_pStmt);
		}

		if (m_pPool && m_pConnection)
		{
			m_pPool->releaseConnection(m_pConnection);
		}
	}
	catch (oracle::occi::SQLException ea)
	{
	}	

	CResultSetCounter::Instance()->ReduceCounter();
}

void CResultSet::SetRecordSize(unsigned int p_uiSize)
{
	m_uiRecordSize = p_uiSize;
}

bool CResultSet::IsValid()
{
	return m_bValid;
	//return m_pResultSet && m_pResultSet->status() == oracle::occi::ResultSet::DATA_AVAILABLE;
}

bool CResultSet::Next()
{	
	if (m_pResultSet)
	{
		oracle::occi::ResultSet::Status status;
		try
		{
			status = m_pResultSet->next();
		}
		catch (oracle::occi::SQLException ea)
		{
			return false;
		}
		
		return status != oracle::occi::ResultSet::Status::END_OF_FETCH;
	}
	return false;
}

unsigned int CResultSet::FieldSize()
{
	return m_mapFieldIndex.size();
}

unsigned int CResultSet::RecordSize()
{
	return m_uiRecordSize;
}

unsigned int CResultSet::GetCurRow()
{
	return m_uiCurRow;
}

bool CResultSet::SetCurRow(unsigned int p_uiCurRow)
{
	if (p_uiCurRow>=m_uiRecordSize)
	{
		return false;
	}
	m_uiCurRow = p_uiCurRow;
	return true;
}

std::vector<std::string> CResultSet::GetFieldNames()
{
	return m_vecFieldNames;
}

//std::string CResultSet::GetValue(unsigned int p_uiRow, unsigned int p_uiCol, const std::string p_strDefault /*= ""*/)
//{
//	const char* l_temp = PQgetvalue(m_pResult.get(), p_uiRow, p_uiCol);
//	std::string l_strResult = l_temp ? l_temp : "";
//	return l_strResult.empty() ? p_strDefault : l_strResult;
//}

std::string CResultSet::GetValue(const std::string p_strFieldName, const std::string p_strDefault /*= ""*/)
{	
	std::map<std::string, unsigned int>::const_iterator itr;
	itr = m_mapFieldIndex.find(p_strFieldName);
	if (itr == m_mapFieldIndex.end())
	{
		return "";
	}

	std::map<std::string, int>::const_iterator itrDataType;
	itrDataType = m_mapDataType.find(p_strFieldName);
	if (itrDataType == m_mapDataType.end())
	{
		return "";
	}

	

	if (itr->second != 0 &&itr->second < 1000 && m_pResultSet)
	{		
		switch (itrDataType->second)
		{
		case SQLT_TIMESTAMP:
		{
			oracle::occi::Timestamp timestamp = m_pResultSet->getTimestamp(itr->second);
			if (timestamp.isNull())
			{
				return "";
			}
			return timestamp.toText("yyyy-mm-dd hh24:mi:ss", 0);
		}
		break;

		default:
			return  m_pResultSet->getString(itr->second);;
			break;
		} 	
		
	}
	return "";
}

std::string CResultSet::GetErrorMsg()
{
	return m_strErrorMsg;
}

void CResultSet::SetErrorMsg(std::string p_strMsg)
{
	m_strErrorMsg = p_strMsg;
}

void CResultSet::SetResultStatus(bool p_bIsValid)
{
	m_bValid = p_bIsValid;
}

void CResultSet::AddFieldIndex(std::string p_strFieldName, unsigned int p_uiFieldIndex)
{
	m_vecFieldNames.push_back(p_strFieldName);
	m_mapFieldIndex[p_strFieldName] = p_uiFieldIndex;
}

void CResultSet::SetSQL(std::string p_strSQL)
{
	m_strSQL = p_strSQL;
}

std::string CResultSet::GetSQL()
{
	return m_strSQL;
}

void CResultSet::SetConnection(oracle::occi::StatelessConnectionPool* pPool, oracle::occi::Connection* pConn)
{
	m_pPool = pPool;
	m_pConnection = pConn;
}

void CResultSet::SetStatement(oracle::occi::Statement* pStmt)
{
	m_pStmt = pStmt;
}

void CResultSet::SetResultSet(oracle::occi::ResultSet* pSet)
{
	m_pResultSet = pSet;
	
	if (IsValid() && m_pResultSet)
	{
		std::vector<oracle::occi::MetaData> data = m_pResultSet->getColumnListMetaData();
		for (unsigned int i = 0; i < data.size(); ++i)
		{
			std::string strClounName = data[i].getString(oracle::occi::MetaData::ATTR_NAME);
			AddFieldIndex(strClounName, i + 1);
			m_mapDataType.insert(std::make_pair(strClounName, data[i].getInt(oracle::occi::MetaData::ATTR_DATA_TYPE)));
		}
	}
	
}

void CResultSet::SetValid(bool bFlag)
{
	m_bValid = bFlag;
}
