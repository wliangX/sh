#pragma once

#define ICCFactoryHeaderInitialize(ImplClass) \
public:\
	ImplClass(IResourceManagerPtr p_IResourceManagerPtr);\
	virtual~ImplClass();\
private:\
	IResourceManagerPtr m_IResourceManagerPtr;

#define ICCFactoryCppInitialize(ImplClass) \
	ImplClass::ImplClass(IResourceManagerPtr p_IResourceManagerPtr) : m_IResourceManagerPtr(p_IResourceManagerPtr) {}\
	ImplClass::~ImplClass(){}
