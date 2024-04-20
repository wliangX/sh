#pragma once

#define ICCGetResource(ImplClass,ResName) \
boost::dynamic_pointer_cast<ImplClass>(this->GetResourceManager()->GetResource(ResName))

#define ICCGetResourceEx(ImplClass,ResName,RM) \
boost::dynamic_pointer_cast<ImplClass>(RM->GetResource(ResName))
