#pragma once
#include <vector>
#include <string>
#include <IObject.h>

namespace ICC
{
	namespace DataBase
	{
		class IResultSet :
			public IObject
		{
		public:
			//结果有效，无效表示SQL执行失败
			virtual bool IsValid() = 0;

			//无可用结果时返回false,有可用结果返回true
			virtual bool Next() = 0;

			//返回结果集中的列数
			virtual unsigned int FieldSize() = 0;

			//返回结果集中的记录数（行数)
			virtual unsigned int RecordSize() = 0;

			//获取当前行的索引值
			virtual unsigned int GetCurRow() = 0;

			//设置当前处理的行数,用户必须保证设置的行值小于结果集中的行数，否则在取值时会崩溃
			virtual bool SetCurRow(unsigned int p_uiCurRow) = 0;

			//获取结果集所有的列名
			virtual std::vector<std::string> GetFieldNames() = 0;

			//获取第iRow行，iCol列的结果，用户需要保证iRow与iCol不越界
			virtual std::string GetValue(unsigned int p_uiRow, unsigned int p_uiCol,const std::string p_strDefault = "") = 0;

			//获取第iRow行，字段名为FieldName的内容
			virtual std::string GetValue(unsigned int p_uiRow, const std::string p_strFieldName, const std::string p_strDefault = "") = 0;

			//取当前行字段名为p_strFieldName的值，一般匹配Next()遍历结果集，单独使用时必须先调用SetCurRow设置当前行
			virtual std::string GetValue(const std::string p_strFieldName,const std::string p_strDefault = "") = 0;

			//设置内部执行的SQL
			//virtual void SetSQL(std::string p_strSQL) = 0;

			//返回内部执行的SQL
			virtual std::string GetSQL() = 0;

			//获取执行结果的错误信息，当IsValid返回false时可调用该函数返回具体错误信息，以便定位错误
			virtual std::string GetErrorMsg() = 0;

			//设置ErrorMsg
			virtual void SetErrorMsg(std::string p_strErrorMsg) = 0;	

		};
		typedef boost::shared_ptr<IResultSet> IResultSetPtr;
	}
}