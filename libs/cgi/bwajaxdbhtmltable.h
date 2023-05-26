//
// C++ Interface: bwajaxdbhtmltable
//
// Description: 
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2013
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CGIBWAJAXDBHTMLTABLE_H
#define CGIBWAJAXDBHTMLTABLE_H

#include <bwajaxhtmltable.h>
#include <bwdatabase.h>
#include <cmath>
#include <bwaccessprofile.h>

namespace CGI {

/**
Classe responsavel por carregar dados de um banco de dados em uma tabela HTML

	@author Bruno Moreira Guedes
*/
class BWAjaxDbHtmlTable : public CGI::BWAjaxHtmlTable
{
public:
    BWAjaxDbHtmlTable(BWKeyPair &, BWConfig &cfg, BWDb &db);
    BWAjaxDbHtmlTable(BWKeyPair &, const BWString &cfg, BWDb &db);

    ~BWAjaxDbHtmlTable();
    BWString GetInnerHTML();

protected:
    BWDb* m_db;
protected:
    void prepareHtmlTable();
    BWString generateQuery(bool justowner = false);
    BWString getTotalPages();
};

}

#endif
