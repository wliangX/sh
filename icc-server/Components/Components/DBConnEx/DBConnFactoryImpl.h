#pragma once

namespace ICC
{
	namespace DataBase
	{
		class CDBConnExFactoryImpl :
			public IDBConnFactory
		{
			ICCFactoryHeaderInitialize(CDBConnExFactoryImpl)
		public:
			virtual IDBConnPtr CreateDBConn(DBType p_iDBType = PostgreSQL);	
			virtual IDBConnPtr CreateNewDBConn(DBType p_iDBType = PostgreSQL);
		private:
			static IDBConnPtr& Instance(IResourceManagerPtr p_pResourceManagerPtr, DBType p_iDBType = PostgreSQL);
		private:
			static boost::mutex sm_Mutex;
			IDBConnPtr m_pDBConn;
			static IDBConnPtr sm_pDBConn;
		};
	}
}