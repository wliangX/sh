#pragma once

#include "Boost.h"

#include "PluginResource.h"

using namespace std;

namespace ICC
{
	class AddressInteraction
	{
	public:
		void Init(boost::shared_ptr<PluginResource> theRES);

		bool GetRealFileServiceFromNacos(string& fileServAddr, string& fileServPort);

		bool GetHealthyServiceFromNacos(const string& servName, string& servAddr, string& servPort);

	public:
		AddressInteraction();
		virtual ~AddressInteraction();


	protected:
		bool ParseNacosResponse(const std::string& theResponseString, string& fileServAddr, string& fileServPort);

		boost::shared_ptr<PluginResource> m_pRES;

		std::string nacosAddress;	  //nacosµÿ÷∑
		std::string nacosPort;
		std::string nacosNameSpaceID;
		std::string nacosGroupName;
		std::string nacosAccessAPI;
	};

}

extern boost::shared_ptr<AddressInteraction> GetAddressInteractionGlobalInstance();
