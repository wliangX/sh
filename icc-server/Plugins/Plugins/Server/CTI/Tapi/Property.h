#pragma once


#include <string>
#include <vector>

namespace ICC
{
	/** 属性*/
	typedef struct CProperty
	{
		std::string m_strKey;		/**< 属性名 */
		std::string m_strValue;		/**< 属性值 */
	}E_PROPERTY;

	inline bool GetPropertyValue(std::vector<CProperty> p_vProperties,
		const std::string& p_strKey, std::string& p_strValue)
	{
		bool l_bRet = false;

		auto it = p_vProperties.cbegin();
		while (it != p_vProperties.cend())
		{
			if (it->m_strKey.compare(p_strKey) == 0)
			{
				l_bRet = true;
				p_strValue = it->m_strValue;

				break;;
			}

			++it;
		}

		return l_bRet;
	}

	inline bool SetPropertyValue(std::vector<CProperty> &vProperties,
		const std::string& strPropertyKey, const std::string& strPropertyValue)
	{
		bool bFind = false;

		auto it = vProperties.begin();
		while (it != vProperties.end())
		{
			if ((*it).m_strKey.compare(strPropertyKey) == 0)
			{
				(*it).m_strValue = strPropertyValue;

				bFind = true;
				break;
			}

			++it;
		}

		if (!bFind)
		{
			CProperty property;
			property.m_strKey = strPropertyKey;
			property.m_strValue = strPropertyValue;

			vProperties.push_back(property);
		}

		return true;
	}

}	// end namespace