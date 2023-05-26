#include "bwconfig.h"

BWConfig::BWConfig() : BWKeyPair()
{
	m_comment = "#";
}

BWConfig::BWConfig(const BWString &file) : BWKeyPair()
{
	m_comment = "#";

	Load(file);
}

BWConfig::~BWConfig()
{
}

#include <iostream>


void BWConfig::Load(const BWString &cfg) {
	m_cfg = cfg;
	BWString line;
	fstream config(cfg.c_str(), fstream::in);

	while (getline(config, line)) {

		if (line == "" || line.compare(0, m_comment.size(), m_comment) == 0) {
			continue;
		}

		kpInsert(line, "=", "\"");
	}	
}


/*!
    \fn BWConfig::GetPath()
 */
BWString BWConfig::GetPath()
{
	return m_cfg;
}


/*!
    \fn BWConfig::SaveDump(const BWString &fname)
 */
void BWConfig::SaveDump(const BWString &fname)
{
     fstream dump(fname.c_str(), fstream::out);
     BWConfig::iterator itr = this->begin();

     while (itr != this->end()) {
            dump << itr->first << "=\"" << itr->second << "\"\n";
            itr++;
     }

     dump.flush();
     dump.close();
}
