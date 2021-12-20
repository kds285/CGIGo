#ifndef SGFTAG_H
#define SGFTAG_H

#include "BasicType.h"

class SgfTag {
private:
	map<string, string> m_sgfTag;

public:
	SgfTag() { reset(); }

	void reset()
	{
		m_sgfTag.clear();
		setSgfTag("KM", ToString(WeichiConfigure::komi));
		setSgfTag("SZ", ToString(WeichiConfigure::BoardSize));
	}

	void setSgfTag(string sTag, string sValue) { m_sgfTag[sTag] = sValue; }
	string getSgfTag(string sTag) const { return (m_sgfTag.find(sTag) == m_sgfTag.end()) ? "" : m_sgfTag.at(sTag); }
	string getSgfTagString()
	{
		ostringstream oss;
		for (map<string, string>::iterator it = m_sgfTag.begin(); it != m_sgfTag.end(); ++it) {
			oss << it->first << "[" << it->second << "]";
		}

		return oss.str();
	}
};

#endif