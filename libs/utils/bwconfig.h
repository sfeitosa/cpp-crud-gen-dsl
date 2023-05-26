//
// C++ Interface: bwconfig
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWCONFIG_H
#define BWCONFIG_H

#include <bwkeypair.h>
#include <bwstring.h>
#include <fstream>

using std::fstream;

/**
Classe responsavel por manipular arquivos de configuracao.

 @author Samuel da Silva Feitosa <samuel@beanweb.com.br>
*/

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
