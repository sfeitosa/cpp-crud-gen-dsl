//
// C++ Interface: bwdbhtmltable
//
// Description: 
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CGIBWDBHTMLTABLE_H
#define CGIBWDBHTMLTABLE_H

#include <bwhtmltable.h>
#include <bwdatabase.h>

namespace CGI {

/**
Classe responsavel por carregar dados de um banco de dados em uma tabela HTML

	@author Bruno Moreira Guedes
*/
class BWDbHtmlTable : public CGI::BWHtmlTable
{
public:
    BWDbHtmlTable(BWConfig& cfg, BWHttp *channel, BWDb &db);
    BWDbHtmlTable(const BWString& cfg, BWHttp *channel, BWDb &db);

    ~BWDbHtmlTable();
    BWString GetInnerHTML();

protected:
    BWDb* bw_db;
protected:
    void prepareHtmlTable();
};

}

#endif
