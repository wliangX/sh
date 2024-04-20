#pragma warning(disable:4996)
#include "Boost.h"
#include "SqlBuilderImpl.h"
#include "SqlRequestImpl.h"
using namespace std;

CSqlBuilderImpl::CSqlBuilderImpl(IResourceManagerPtr p_pResourceManager) : m_pResourceManager(p_pResourceManager)
{
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);	
	m_pConfigConfig = ICCGetIConfigFactory()->CreateConfigConfig();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_lock = ICCGetILockFactory()->CreateLock(Lock::TypeRecursiveMutex);

	ReadConfig();
}

CSqlBuilderImpl::~CSqlBuilderImpl()
{

}

//************************************************************************
// ����:  ReadConfig
// ȫ��:  CSqlBuilderImpl::ReadConfig
// ����:  load and check the configuration file  according to p_strConfigPath
// ����:  virtual public 
// ����:  string p_strCfgPathOrContent,file path of config file or the config content string
// ����:  unsigned int, a code to indicate the result
//************************************************************************
unsigned int CSqlBuilderImpl::ReadConfig()
{
	Lock::AutoLock l_lock(m_lock);

	if (!m_mapSqlElem.empty())
	{
		return SQLBD_SUCCESS;
	}

	std::string l_strSqlConfigPath = m_pConfigConfig->GetValue("ICC.SqlConfigPath", "./Config/sql.xml");
	if (m_oDocCfg.LoadFile(l_strSqlConfigPath.c_str()) != tinyxml2::XML_SUCCESS
		&&m_oDocCfg.Parse(l_strSqlConfigPath.c_str()) != tinyxml2::XML_SUCCESS)
	{
		ICC_LOG_ERROR(m_pLog, "Load config file failed : %s", m_oDocCfg.ErrorStr());
		return SQLBD_LOAD_CFG_FAILED;
	}

	return CheckSqlConfig(m_oDocCfg);
}

//************************************************************************
// ����:  BuildSql
// ȫ��:  CSqlBuilderImpl::BuildSql
// ����:  build corresponding SQL according to request
// ����:  virtual public 
// ����:  string p_strXmlSqlRequest,SQL build request in JSON syntax,which supports multiple
//		  request in one json string.The input string should satisfy the application protocol as well:
/*
				{
					"body":
					{
						[
							{
								"sql_id":"select_user_by_id",
								"param" :
								{
									"id"  : "1002",
									"age" : "18",
									"name": "Rose"
								}
							}
						...
						]
					}
				}
*/
// ����:  string & p_strSqlResult,the build result
// ����:  unsigned int,the code that indicates the execution is success or the reason why it is failed
//************************************************************************
unsigned int CSqlBuilderImpl::BuildSql(string p_strSqlRequest, string& p_strSqlResult)
{
	p_strSqlResult.clear();
	JsonParser::IJsonPtr l_pJson = m_pJsonFty->CreateJson();
	SqlRequestListPtr l_pSqlReqLst = boost::make_shared<CSqlRequestList>(l_pJson,m_pString);
	if (!l_pSqlReqLst->Parse(p_strSqlRequest))
	{
		return SQLBD_REQ_NOT_JSON;
	}	
	vector<ISqlRequestPtr> l_vecRequests = l_pSqlReqLst->GetAllRequest();
	for (auto l_pRequest : l_vecRequests)
	{
		unsigned int l_uiRetCode = this->BuildSql(l_pRequest, p_strSqlResult);
		if (l_uiRetCode!=SQLBD_SUCCESS)
		{
			return l_uiRetCode;
		}		
	}	
	return SQLBD_SUCCESS;
}

unsigned int CSqlBuilderImpl::BuildSql(ISqlRequestPtr p_pSqlRequest, std::string& p_strSqlResult)
{
	Lock::AutoLock l_lock(m_lock);

	p_strSqlResult.clear();
	string l_strReqId = p_pSqlRequest->GetSqlID();
	if (l_strReqId.empty())
	{
		return SQLBD_REQ_ID_EMPTY;
	}
	auto it = m_mapSqlElem.find(l_strReqId);
	if (it == m_mapSqlElem.end())
	{
		ICC_LOG_ERROR(m_pLog, "[sql_id not matched]request id:%s", l_strReqId.c_str());
		return SQLBD_REQ_ID_ERROR;
	}

	//����������ʼ�ĵط�
	if (!l_strReqId.compare("begin") || !l_strReqId.compare("commit") || !l_strReqId.compare("rollback"))
	{//����
		p_strSqlResult = l_strReqId;
		return SQLBD_SUCCESS;
	}

	tinyxml2::XMLElement* l_pSqlElem = it->second;//request_id����Ӧ��SQL�������ָ��,<sql>��ǩ
	string l_strOpType = GetAttribute(l_pSqlElem, "operation");
	if (l_strOpType.compare("insert") == 0)
	{// insert
		p_strSqlResult.append(this->BuildInsert(l_pSqlElem, p_pSqlRequest));
	}
	else if (l_strOpType.compare("update") == 0)
	{// update
		p_strSqlResult.append(this->BuildUpdate(l_pSqlElem, p_pSqlRequest));
	}
	else
	{// select , delete , function and others
		p_strSqlResult.append(this->BuildOthers(l_pSqlElem, p_pSqlRequest));
	}

	if (p_strSqlResult.empty())
	{
		return SQLBD_EMPTY_RESULT;
	}

	p_strSqlResult.append(";");//�Էֺŷָ����SQL���
	return SQLBD_SUCCESS;
}

//************************************************************************
// ����:  GetErrorMsg
// ȫ��:  CSqlBuilderImpl::GetErrorMsg
// ����:  transform an unsigned int error code to a string description
// ����:  virtual public 
// ����:  unsigned int p_uiErrorCode, error code returned by other functions
// ����:  string  a string description of the input error code
//************************************************************************
string CSqlBuilderImpl::GetErrorMsg(unsigned int p_uiErrorCode)
{
	switch (p_uiErrorCode)
	{
	case SQLBD_SUCCESS:
		return "Success";
	case SQLBD_EMPTY_RESULT:
		return "Empty Build Result";
	case SQLBD_REQ_NOT_JSON:
		return "Request Syntax Error [Not Json]";
	case SQLBD_REQ_ID_EMPTY:
		return "No Request SQL ID ";
	case SQLBD_REQ_ID_ERROR:
		return "SQL ID Not Matched";
	case SQLBD_CFG_ID_EMPTY:
		return "Found No SQL ID In Config Item";
	case SQLBD_CFG_FILE_EMPTY:
		return "Empty Config File";
	case SQLBD_CFG_ID_REPEATED:
		return "Repeated SQL ID In Config File";
	case SQLBD_CFG_TEXT_EMPTY:
		return "Empty Config Item Statement";
	case SQLBD_CFG_TEXT_SYNTAX_ERROR:
		return "Syntax Error Config Item Statement";
	case SQLBD_LOAD_CFG_FAILED:
		return "Failed to Load Configuration File";
	default:
		return "Unknown Error Code : " + std::to_string(p_uiErrorCode);
	}
}

/*
 * ����: CSqlBuilderImpl::BuildSelect
 * ����: ����ʹ��
 * ����: tinyxml2::XMLElement * p_pSqlElem ,ָ��XML�����ļ���sqlԪ��
 * ����: ISqlRequestPtr p_oRequest
 * ����: std::string
 * ���ߣ�[3/2/2018 t26150]
*/
std::string CSqlBuilderImpl::BuildOthers(tinyxml2::XMLElement* p_pSqlElem, ISqlRequestPtr p_pRequest)
{
	if (!p_pSqlElem)
	{
		return "";
	}

	string l_strResult = GetText(p_pSqlElem->FirstChildElement("statement"));
	if (!p_pRequest->GetParam().empty())
	{
		map<string, string> p_mapParams = p_pRequest->GetParam()[0];
		string l_strFieldIsNotTrans(GetAttribute(p_pSqlElem->FirstChildElement("statement"), "isNotTrans"));

		ReplacePlaceHolder(l_strResult, p_mapParams, true, l_strFieldIsNotTrans == "1");
		boost::algorithm::trim_if(l_strResult, boost::is_any_of("\n\t"));
		//ָ��sql�������еĶ�̬����Ԫ�� ��dynamic,set,limits,offset,order by �ȵ�
		//<statement>Ϊ<sql>�����һ��Ԫ��
		tinyxml2::XMLElement* l_pDynamic(nullptr);
		if (p_pSqlElem->FirstChildElement())
		{
			l_pDynamic = p_pSqlElem->FirstChildElement()->NextSiblingElement();
		}
		while (l_pDynamic)
		{
			string l_strDynamicResult;
			tinyxml2::XMLElement* l_pChild = l_pDynamic->FirstChildElement();
			if (l_pChild)
			{
				l_strDynamicResult = BuildDynamic(l_pChild, p_mapParams);
			}

			if (!l_strDynamicResult.empty())
			{
				l_strResult.append(" ").append(GetAttribute(l_pDynamic, "prepend")).append(" ")
					.append(l_strDynamicResult);
			}

			string l_strElemSuffixText;
			tinyxml2::XMLText* l_pText = l_pDynamic->LastChild()->ToText();
			if (l_pText&&l_pText->Value())
			{
				l_strElemSuffixText = string(l_pText->Value());
				boost::algorithm::trim_if(l_strElemSuffixText, boost::is_any_of("\n\t"));
			}
			l_strResult.append(l_strElemSuffixText);
			l_pDynamic = l_pDynamic->NextSiblingElement();
		}		
	}		
	return l_strResult;
}

//************************************
// Method:    BuildInsert
// FullName:  CSqlBuilderImpl::BuildInsert
// Access:    private 
// Returns:   string
// Qualifier:
// Parameter: tinyxml2::XMLElement * p_pSqlItem
// Parameter: vector<map<string,string>> & p_pReqParams, request parameters <col_name,col_val>
//************************************
string CSqlBuilderImpl::BuildInsert(tinyxml2::XMLElement* p_pSqlItem, ISqlRequestPtr p_pRequest)
{	
	string l_strInsert(GetText(p_pSqlItem->FirstChildElement("statement")));
	boost::trim(l_strInsert);
	map<string, string> p_mapParams;
	if (!p_pRequest->GetParam().empty())
	{
		p_mapParams = p_pRequest->GetParam()[0];
		ReplacePlaceHolder(l_strInsert, p_mapParams);
	}
		
	//��̬���첿��
	tinyxml2::XMLElement* l_pDynamic = p_pSqlItem->FirstChildElement("dynamic");
	std::string l_strDynamicText = GetText(l_pDynamic);
	int l_iLeftBrace = l_strDynamicText.rfind("{");
	int l_iRightBrace = l_strDynamicText.rfind("}");
	while (l_iLeftBrace >= 0 && l_iRightBrace >= 0)
	{
		string l_strParamName = l_strDynamicText.substr(l_iLeftBrace + 1, l_iRightBrace - l_iLeftBrace - 1);
		auto iter = p_mapParams.find(l_strParamName);
		if (iter == p_mapParams.end() || iter->second.empty())
		{
			l_strDynamicText = m_pString->ReplaceAll(l_strDynamicText, "'{" + l_strParamName + "}'", "NULL");
		}
		else
		{
			string l_strValue = m_pString->ReplaceAll(iter->second, "'", "''");
			l_strDynamicText = m_pString->ReplaceAll(l_strDynamicText, "{" + l_strParamName + "}", l_strValue);
		}

		l_iRightBrace = l_strDynamicText.rfind('}', l_iLeftBrace - 1);
		l_iLeftBrace = l_strDynamicText.rfind('{', l_iRightBrace - 1);
	}
	
	l_strInsert.append(" " + l_strDynamicText);
	//��̬���첿��,�ڶ���Dynamic
	l_pDynamic = l_pDynamic->NextSiblingElement();
	while (l_pDynamic)
	{
		// what the values clause looks like >>> l_strValueClauseCfg = "('{id}','{name}','{address}',age)"
		string l_strCfgValueClause(GetText(l_pDynamic));
		if (!l_strCfgValueClause.empty())
		{
			ReplacePlaceHolder(l_strCfgValueClause, p_mapParams);
			l_strInsert.append(" " + GetAttribute(l_pDynamic, "prepend") + " ").append(l_strCfgValueClause);
		}

		string l_strDynamicResult;
		tinyxml2::XMLElement* l_pChild = l_pDynamic->FirstChildElement();
		if (l_pChild)
		{
			l_strDynamicResult = BuildDynamic(l_pChild, p_mapParams);
		}
		if (!l_strDynamicResult.empty())
		{
			l_strInsert.append(" ").append(GetAttribute(l_pDynamic, "prepend")).append(" ")
				.append(l_strDynamicResult);

			string l_strElemSuffixText;
			tinyxml2::XMLText* l_pText = l_pDynamic->LastChild()->ToText();
			if (l_pText&&l_pText->Value())
			{
				l_strElemSuffixText = string(l_pText->Value());
				boost::algorithm::trim_if(l_strElemSuffixText, boost::is_any_of("\n\t"));
			}
			l_strInsert.append(l_strElemSuffixText);
		}
				
		l_pDynamic = l_pDynamic->NextSiblingElement();
	}	
	return l_strInsert;
}

//************************************
// Method:    BuildDynamic
// FullName:  CSqlBuilderImpl::BuildDynamic
// Access:    private 
// Returns:   string
// Qualifier: determine which condition expression was used for building where clause
//			  according to request parameters and SQL config file.
// Parameter: tinyxml2::XMLElement * l_pDynamic,a tinyxml2::XMLElement pointer which points to the dynamic child tinyxml2::XMLElement
//			  in config file <dynamic>��</dynamic>
// Parameter: map<string,string> & p_mapParams, a map contains parameter info
//************************************
string CSqlBuilderImpl::BuildDynamic(tinyxml2::XMLElement* l_pDynamicChild,const map<string, string>& p_mapParams,string p_strPrepend)
{
	string l_strResult("");
	while (l_pDynamicChild)
	{
		string l_strDynamic(GetText(l_pDynamicChild));
		string l_strFieldIsString(GetAttribute(l_pDynamicChild, "isString"));

		string l_strFieldIsNotTrans(GetAttribute(l_pDynamicChild, "isNotTrans")); //�Ƿ�ת������
		if (!l_strDynamic.empty() && ReplacePlaceHolder(l_strDynamic, p_mapParams, l_strFieldIsString == "1", l_strFieldIsNotTrans == "1"))
		{
/*
			unsigned int l_uiLeftBrace = l_strDynamic.find_first_of("{");
			unsigned int l_uiRightBrace = l_strDynamic.find_first_of("}");
			string l_strParamName = l_strDynamic.substr(l_uiLeftBrace + 1, l_uiRightBrace - l_uiLeftBrace - 1);
			auto iter = p_mapParams.find(l_strParamName);
			if (iter != p_mapParams.end())
			{
				string l_strParamValue(iter->second);
				string l_strFieldIsString(GetAttribute(l_pDynamicChild, "isString"));
				l_strParamValue = m_pString->ReplaceAll(l_strParamValue, "'", "''");//�������֧�ֵ�����
				if (iter->second.empty())
				{
					if (l_strFieldIsString != "1")
					{//�ַ����ֶ��滻Ϊ���ַ���
						l_strDynamic.replace(l_uiLeftBrace, l_uiRightBrace - l_uiLeftBrace + 1, l_strParamValue);
					}
					else
					{//���ַ����ֶ�						
						l_strDynamic = m_pString->ReplaceAll(l_strDynamic, "'{" + l_strParamName + "}'", "NULL");
					}
				}
				else
				{
					l_strDynamic.replace(l_uiLeftBrace, l_uiRightBrace - l_uiLeftBrace + 1, l_strParamValue);
				}
			}*/
					
			//ReplacePlaceHolder(l_strDynamic, p_mapParams, l_strFieldIsString == "1" );

			if (l_strResult.empty())
			{
				l_strResult = l_strDynamic;
			}
			else if (p_strPrepend.empty())
			{
				l_strResult = l_strResult + " " + GetAttribute(l_pDynamicChild, "prepend") + " " + l_strDynamic;
			}
			else
			{
				l_strResult = l_strResult + p_strPrepend + " " + l_strDynamic;
			}
		}
		l_pDynamicChild = l_pDynamicChild->NextSiblingElement();
	}
	return l_strResult;
}

//************************************
// Method   : BuildUpdate
// FullName : CSqlBuilderImpl::BuildUpdate
// Function : build update statements dynamically,the result varies 
//			  depending on the request parameters
// Access   : private
// Returns  : std::string
// Parameter: tinyxml2::XMLElement * p_pSqlElem
// Parameter: SqlRequestPtr p_pRequest
//************************************
string CSqlBuilderImpl::BuildUpdate(tinyxml2::XMLElement* p_pSqlElem, ISqlRequestPtr p_pRequest)
{
	string l_strUpdateClause(GetText(p_pSqlElem->FirstChildElement("statement")));
	boost::trim(l_strUpdateClause);
	if (l_strUpdateClause.empty())
	{
		ICC_LOG_ERROR(m_pLog, "[Empty update clause configuration]");
		return "";
	}
	
	//set clause,�����ϰ汾��SET��ֵ����SQLRequest������set map����
	const map<string, string> & l_mapSetNewVal = p_pRequest->GetNewValParam();
	if (l_mapSetNewVal.empty())
	{
		ICC_LOG_WARNING(m_pLog, "[No \"set\" parameters found when build \"UPDATE\" clause]");
	}

	tinyxml2::XMLElement* l_pDynamic = p_pSqlElem->FirstChildElement("dynamic");	
	tinyxml2::XMLElement* l_pDynamicSet = l_pDynamic;
	while (l_pDynamicSet)
	{
		//ʹ��SQLRequest set�����������ʱ��set�Ӿ���where����ͬһ���ö�
		if (GetAttribute(l_pDynamicSet, "prepend") == "where")
		{
			tinyxml2::XMLElement* l_pDynamicChild = l_pDynamicSet->FirstChildElement();
			std::string l_strSetClause(BuildDynamic(l_pDynamicChild, l_mapSetNewVal, ","));
			if (!l_strSetClause.empty())
			{
				l_strUpdateClause.append(" set ").append(l_strSetClause);
			}
			break;
		}			
		l_pDynamicSet = l_pDynamicSet->NextSiblingElement();
	}
	
	//where clause ��set clause,�°汾��SET��ֵ����SQLRequest������set,��������param
	const vector<map<string, string>> l_mapConditionParams = p_pRequest->GetParam();
	if (!l_mapConditionParams.empty())
	{
		ReplacePlaceHolder(l_strUpdateClause, l_mapConditionParams[0]);
		while (l_pDynamic)
		{
			std::string l_strDynamicClause(BuildDynamic(l_pDynamic->FirstChildElement(), l_mapConditionParams[0]));
			if (!l_strDynamicClause.empty())
			{
				l_strUpdateClause.append(" ")
					.append(GetAttribute(l_pDynamic, "prepend"))
					.append(" ")
					.append(l_strDynamicClause);
			}
			l_pDynamic = l_pDynamic->NextSiblingElement();
		}
	}

	return l_strUpdateClause;
}

//************************************
// Method   : ReplacePlaceHolder
// FullName : CSqlBuilderImpl::ReplacePlaceHolder
// Access   : private 
// Returns  : bool
// Function : replace the placeholders in p_strSource with corresponding value in p_mapParams
// Parameter: string & p_strSource
// Parameter: const map<string,string>& p_mapParams
// Parameter: bool isFieldString true when the type of field is string
// Parameter: string p_strDefaults
//************************************
bool CSqlBuilderImpl::ReplacePlaceHolder(string& p_strSource,const map<string, string>& p_mapParams,
	bool isFieldString/*=false*/, bool isNotTrans, string p_strDefaults/*=""*/)
{
	//������������滻�����е�ռλ�����Ա�������а���{}�ȷ��ŵ�Ӱ��
	int l_iLeftBrace = p_strSource.rfind("{");
	int l_iRightBrace = p_strSource.rfind("}");	
	while (l_iLeftBrace >= 0 && l_iRightBrace >= 0)
	{
		string l_strParamName = p_strSource.substr(l_iLeftBrace + 1, l_iRightBrace - l_iLeftBrace - 1);
		auto iter = p_mapParams.find(l_strParamName);
		if (iter == p_mapParams.end())
		{
			return false;
		}

		string l_strParamValue(iter->second);

		if (!isNotTrans) //�Ƿ���Ҫת�����ţ�Ĭ�϶�ת��ֻ��������isNotTrans = "1"����ת
		{
			l_strParamValue = m_pString->ReplaceAll(l_strParamValue, "'", "''");//�������֧�ֵ�����
		}
		
		if (iter->second.empty())
		{
			if (isFieldString)
			{//�ַ����ֶ��滻Ϊ���ַ���
				p_strSource.replace(l_iLeftBrace, l_iRightBrace - l_iLeftBrace + 1, l_strParamValue);
			}
			else
			{//���ַ����ֶε�ֵ����ʱ��ȣ������ɿ��ַ���ת����ֻ���滻��NULL
				p_strSource = m_pString->ReplaceAll(p_strSource, "'{" + l_strParamName + "}'", "NULL");
			}
		}
		else
		{
			p_strSource.replace(l_iLeftBrace, l_iRightBrace - l_iLeftBrace + 1, l_strParamValue);
		}

		l_iRightBrace = p_strSource.rfind("}", l_iLeftBrace - 1);
		l_iLeftBrace = p_strSource.rfind("{", l_iRightBrace - 1);
	}
	return true;
}


/*
* SQLBuilder���ع��̵������ļ���⺯��
* 1. ���sql id�Ƿ��ظ���SQLBuilder��id��ʶΨһsql����
* 2. ���statement�Ƿ�Ϊ��
* 3. ����������е�ռλ��'{'��'}'�Ƿ�ƥ��
*
* ��������ӡ����־�У��ɸ�����־��ʾ�޸�sql.xml�����ļ�
*/
unsigned int CSqlBuilderImpl::CheckSqlConfig(tinyxml2::XMLDocument& p_oDoc)
{
	if (!p_oDoc.FirstChild())
	{
		return SQLBD_CFG_FILE_EMPTY;
	}
	tinyxml2::XMLElement* l_pSqlElem = p_oDoc.FirstChild()->FirstChildElement("sql");
	while (l_pSqlElem)
	{
		//check whether the sql id is empty or not
		string l_strSqlid = GetAttribute(l_pSqlElem, "id");
		if (!l_strSqlid.empty())
		{
			if (m_mapSqlElem.find(l_strSqlid) != m_mapSqlElem.end())
			{
				//if there is already a same id in the map,error
				ICC_LOG_ERROR(m_pLog, "Repeated ID,  line %d", l_pSqlElem->GetLineNum());				
				return SQLBD_CFG_ID_REPEATED;
			}
			//insert pair <id,tinyxml2::XMLElement* sql> into map for later use
			m_mapSqlElem[l_strSqlid] = l_pSqlElem;
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "Unknown ID , line %d", l_pSqlElem->GetLineNum());
			return SQLBD_CFG_ID_EMPTY;
		}
		//check whether if the sql text is empty or nots
		tinyxml2::XMLElement* l_pStatement = l_pSqlElem->FirstChildElement("statement");
		if (l_pStatement && !l_pStatement->GetText())
		{
			ICC_LOG_ERROR(m_pLog, "Empty Text , line %d", l_pSqlElem->GetLineNum());
			return SQLBD_CFG_TEXT_EMPTY;
		}
		//check whether if the braces in sql text are matched or not
		if (!IsBracesMatched(l_pSqlElem))
		{
			return SQLBD_CFG_TEXT_SYNTAX_ERROR;
		}
		l_pSqlElem = l_pSqlElem->NextSiblingElement();
	}
	return SQLBD_SUCCESS;
}

//************************************************************************
// ����:  IsBracesMatched
// ȫ��:  CSqlBuilderImpl::IsBracesMatched
// ����:  check whether if the braces in the SQL config text are matched or not
// ����:  private 
// ����:  string p_strText,the SQL text from xml config file
// ����:  bool,true if braces are matched,or return false
//************************************************************************
bool CSqlBuilderImpl::IsBracesMatched(string p_strText)
{
	int l_iLeftBraceNum = 0;
	for (char l_cChar : p_strText)
	{
		if (l_cChar == '{')
		{
			++l_iLeftBraceNum;
		}
		else if (l_cChar == '}')
		{
			--l_iLeftBraceNum;
		}
		if (l_iLeftBraceNum < 0)
		{
			return false;
		}
	}
	return (l_iLeftBraceNum == 0);
}

bool CSqlBuilderImpl::IsBracesMatched(tinyxml2::XMLElement* p_pXMLElem)
{
	if (IsBracesMatched(GetText(p_pXMLElem)))
	{
		if (!p_pXMLElem->FirstChild())
		{//���ӽڵ�
			return true;
		}
		tinyxml2::XMLElement* l_pChild = p_pXMLElem->FirstChild()->ToElement();
		while (l_pChild && IsBracesMatched(l_pChild))
		{			
			l_pChild = l_pChild->NextSiblingElement();
		}
		return l_pChild == nullptr;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Syntax Error In Configuration File , Line %d", p_pXMLElem->GetLineNum());
		return false;
	}	
}

string CSqlBuilderImpl::GetText(tinyxml2::XMLElement*p_pElem, string p_strDefaultVlaue /*= ""*/)
{
	if (p_pElem&&p_pElem->FirstChild() && p_pElem->FirstChild()->ToText() && p_pElem->FirstChild()->ToText()->Value())
	{
		return string(p_pElem->FirstChild()->ToText()->Value());
	}
	return p_strDefaultVlaue;
}

string CSqlBuilderImpl::GetAttribute(tinyxml2::XMLElement*p_pElem, string p_strAttrName, string p_strDefaultVlaue)
{
	const char* l_cstrAttrValue = p_pElem->Attribute(p_strAttrName.c_str());
	if (l_cstrAttrValue)
	{
		return string(l_cstrAttrValue);
	}
	return p_strDefaultVlaue;
}

IResourceManagerPtr CSqlBuilderImpl::GetResourceManager()
{
	return m_pResourceManager;
}