#pragma once 
#include <Json/IJson.h>
#include <DateTime/IDateTimeFactory.h>
#include <StringUtil/IStringFactory.h>
#include <DBConn/IDBConnFactory.h>

namespace ICC
{
	namespace Data
	{
		class IBaseData
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			//关系型数据库接口
			//////////////////////////////////////////////////////////////////////////
			virtual bool LoadData() = 0;
			virtual int  ReceiveTotalDataSize() = 0;
			virtual void AppendReceiveDataSize(int iReceiveSize) = 0;

			virtual bool SynUpdate() = 0;
			
			virtual bool SingleAdd() = 0;
			virtual bool SingleModify() = 0;
			virtual bool SingleDelete() = 0;			

		public:
			IBaseData(){}
			virtual ~IBaseData(){}
		};

		typedef boost::shared_ptr<IBaseData> IBaseDataPtr;
	}
}
