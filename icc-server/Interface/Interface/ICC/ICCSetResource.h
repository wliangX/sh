#pragma once
#include <list>
#include <string>

#define ICCResourceInitialize \
std::list<std::string> m_ResList

#define ICCSetResource(ImplClass,ResName) \
{ \
	this->GetResourceManager()->SetResource(ResName, boost::make_shared<ImplClass>()); \
	m_ResList.push_front(ResName); \
}

#define ICCSetResourceEx(ImplClass,ResName) \
{ \
	this->GetResourceManager()->SetResource(ResName, boost::make_shared<ImplClass>(this->GetResourceManager())); \
	m_ResList.push_front(ResName); \
}

#define ICCClearResource \
{ \
	for each (std::string res in m_ResList) \
	{ \
		this->GetResourceManager()->SetResource(res, IResourcePtr()); \
	} \
}