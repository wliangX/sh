#pragma once
// #include <IResource.h>
// #include <IObject.h>

namespace ICC
{
	namespace Config
	{
		/*
		* class   配置类
		* author  w26326
		* purpose
		* note    // Root/Node[0]/Node[1]/Node[2]
		*/
		class IConfig :
			public IObject
		{
		public:
			//************************************
			// Method:    GetValue
			// FullName:  ACS::IConfig::GetValue
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier:
			// Parameter: std::string p_strNodePath
			// Parameter: std::string p_strDefault
			//************************************
			virtual std::string GetValue(std::string p_strNodePath, std::string p_strDefault) = 0;
			//************************************
			// Method:    SetValue
			// FullName:  ACS::IConfig::SetValue
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier:
			// Parameter: std::string p_strNodePath
			// Parameter: std::string p_strValue
			//************************************
			virtual bool SetValue(std::string p_strNodePath, std::string p_strValue) = 0;

			//************************************
			// Method:    GetNodeCount
			// FullName:  ACS::Config::IConfig::GetNodeCount
			// Access:    virtual public 
			// Returns:   unsigned int
			// Qualifier:
			// Parameter: std::string p_strNodePath
			// Parameter: std::string p_strNodeName
			//************************************
			virtual unsigned int GetNodeCount(std::string p_strNodePath, std::string p_strNodeName) = 0;

			//读公共配置
			virtual unsigned int GetPubicNodeCount(std::string p_strNodePath, std::string p_strNodeName) { return 0; };
			virtual std::string GetPublicValue(int p_nCfgType, std::string p_strNodePath, std::string p_strDefault) { return ""; };

			virtual void SetServerFlag(const std::string& strServerFlag) {};
			virtual std::string ServerFlag() { return ""; };

			virtual bool ReloadConfig() { return false; };
		};

		typedef boost::shared_ptr<IConfig> IConfigPtr;
	}
}
