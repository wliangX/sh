#pragma once
#include <list>
#include <boost/dll/shared_library.hpp>
#include <boost/filesystem/path.hpp>
#include <IFactory.h>

#define ICCLibInitialize \
std::list<boost::dll::shared_library> m_LibList

#define ICCLibImport(ImplClass,ImplObj,LibDir,LibFile) \
{ \
	boost::filesystem::path LibPath(LibDir); \
	LibPath.append(LibFile); \
	boost::dll::shared_library ImplLib(LibPath); \
	if (ImplLib.has("ICCCreateInstance")) \
	{ \
		IFactoryPtr pFty(ImplLib.get<IFactory*()>("ICCCreateInstance")()); \
		ImplObj = boost::dynamic_pointer_cast<ImplClass>(pFty->CreateObject()); \
		m_LibList.push_front(ImplLib); \
	} \
}

#define ICCLibImportEx(ImplClass,ImplObjList,LibDir,LibExt) \
{ \
	boost::filesystem::directory_iterator itEnd; \
	for (boost::filesystem::directory_iterator it(boost::filesystem::path(LibDir)); it != itEnd; ++it) \
	{ \
		if (boost::filesystem::is_regular_file(*it) \
			&& (*it).path().extension() == boost::filesystem::path(LibExt)) \
		{ \
			printf("import component path = %s\n", (*it).path().string().c_str());\
			boost::filesystem::path LibPath(*it); \
			boost::dll::shared_library ImplLib(LibPath); \
			if (ImplLib.has("ICCCreateInstance")) \
			{ \
				printf("import component path = %s success! will create instance\n", (*it).path().string().c_str());\
				IFactoryPtr pFty(ImplLib.get<IFactory*()>("ICCCreateInstance")()); \
				ImplObjList.push_front(boost::dynamic_pointer_cast<ImplClass>(pFty->CreateObject())); \
				m_LibList.push_front(ImplLib); \
			} \
		} \
	} \
}
