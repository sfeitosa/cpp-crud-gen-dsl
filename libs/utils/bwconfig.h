#ifndef BWCONFIG_H
#define BWCONFIG_H

#include <bwkeypair.h>
#include <bwstring.h>
#include <fstream>

using std::fstream;

class BWConfig: public BWKeyPair {

    public:
        BWConfig();
	BWConfig(const BWString &);

        ~BWConfig();

        void Load(const BWString &);
	BWString GetPath();
    void SaveDump(const BWString &fname);

    private:
        BWString m_cfg;
	BWString m_comment;


};

#endif
