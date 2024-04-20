#pragma once
#include <map>
#include <vector>
#include <string>
#include <IObject.h>

namespace ICC
{
	namespace DataBase
	{
		class ISqlRequest :
			public IObject
		{
		public:
			//SQL ID
			virtual std::string GetSqlID() = 0;
			virtual void SetSqlID(std::string p_strSqlID) = 0;
			//用于构造where语句，insert语句等
			virtual std::vector<std::map<std::string, std::string>>& GetParam() = 0;
			virtual void SetParam(std::string p_strName, std::string p_strValue) = 0;
			virtual void SetParam(const std::map<std::string, std::string>& p_mapParam) = 0;
			virtual void AddParam(const std::map<std::string, std::string>& p_mapParam) = 0;
			virtual void SetParam(const std::vector<std::map<std::string, std::string>>& p_vecParams) = 0;
			//用于构造update set语句
			virtual std::map<std::string, std::string>& GetNewValParam() = 0;
			virtual void SetNewValParam(std::string p_strName, std::string p_strValue) = 0;
			virtual void SetNewValParam(std::map<std::string, std::string>& p_mapNewValParam) = 0;
		};
		typedef boost::shared_ptr<ISqlRequest> ISqlRequestPtr;
	}
}