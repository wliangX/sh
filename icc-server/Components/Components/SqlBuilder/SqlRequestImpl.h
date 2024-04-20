#pragma once
#include <Json/IJson.h>
using namespace std;

/***************************************************************************
				SqlBuidler Request 目前支持的协议格式
/***************************************************************************
{
	"body":
	[
	{
		"sql_id":"select_user",
		"param" :
		{
		}
	},
	{
		"id":"update_user",
		"set" :
		{
			"id":"2036",
			"name":"zhangwuji"
		},
		"param" :
			{
				"id":"1002"
			}
	},
	{
		"id":"select_user_by_id",
		"param" :
		{
			  "id" : "1002",
			 "age" : "18",
			"name" : "lisi"
		}
	}
	]
}
****************************************************************************/

namespace ICC
{
	namespace DataBase
	{
		class CSqlRequest :public ISqlRequest
		{
			typedef std::map<std::string, std::string> param;

		public:
			CSqlRequest();
			~CSqlRequest();
			virtual std::string GetSqlID();
			virtual void SetSqlID(std::string p_strSqlID);

			//用于构造where语句，insert语句等
			virtual std::vector<std::map<std::string, std::string>>& GetParam();
			virtual void AddParam(const std::map<std::string, std::string>& p_mapParam);
			virtual void SetParam(const std::map<std::string, std::string>& p_mapParam);
			virtual void SetParam(std::string p_strName, std::string p_strValue);
			virtual void SetParam(const std::vector<std::map<std::string, std::string>>& p_vecParams);

			//用于构造update set语句
			virtual std::map<std::string, std::string>& GetNewValParam();
			virtual void SetNewValParam(std::string p_strName, std::string p_strValue);
			virtual void SetNewValParam(std::map<std::string, std::string>& p_mapNewValParam);

		private:
			string				m_strSqlID;
			vector<param>		m_vecParam;			//WHERE语句相关的参数 <name,value>,vector用于支持一次插入多条记录的情形
			map<string, string>	m_mapNewValParam;	//UPDATE,SET语句相关的参数 <col_name,new_value>
		};

		//typedef boost::shared_ptr<CSqlRequest> SqlRequestPtr;

		class CSqlRequestList
		{
		public:
			CSqlRequestList(JsonParser::IJsonPtr p_pJson, StringUtil::IStringUtilPtr p_pStrUtil);
			~CSqlRequestList();

			//************************************************************************
			// 方法:  ParseRequest
			// 全名:  ICC::CSqlRequest::ParseRequest
			// 功能:  initialize this request list object with a request string , which
			//  	  should conform the protocol above. 
			// 访问:  virtual public 
			// 参数:  string p_strRequest
			// 返回:  bool
			//************************************************************************
			bool Parse(string p_strRequest);

			void AddRequest(ISqlRequestPtr &p_pParams);
			void AddRequest(vector<ISqlRequestPtr> & p_vecParams);
			void SetRequests(vector<ISqlRequestPtr> & p_vecParams);

			unsigned int GetRequestsCount();
			vector<ISqlRequestPtr>& GetAllRequest();

		private:
			JsonParser::IJsonPtr m_pJson;
			StringUtil::IStringUtilPtr			m_pString;
			vector<ISqlRequestPtr>	m_vecRequests;
		};

		typedef boost::shared_ptr<CSqlRequestList> SqlRequestListPtr;
	}
}