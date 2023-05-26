//
// C++ Interface: BWAjaxHtmlTable
//
// Description: 
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2013
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWAJAXHTMLTABLE_H
#define BWAJAXHTMLTABLE_H

#include <bwconfig.h>
#include <bwkeylist.h>
#include <bwhttp.h>
#include <bwtemplate.h>
#include <bwlist.h>
#include <bwerror.h>

#define _BW_HTML_TABLE "var/www/html/templates/table.html"
#define _BW_HTML_BUTTON "var/www/html/templates/button.html"

/**
Classe responsavel pela formatacao de tabelas HTML

	@author Samuel da Silva Feitosa
*/

using namespace std;
using namespace Http;

namespace CGI {

class BWAjaxHtmlTable {
public:
    BWAjaxHtmlTable(BWKeyPair &, const BWString &cfg);
    BWAjaxHtmlTable(BWKeyPair &, BWConfig &cfg);

    virtual ~BWAjaxHtmlTable() {}
    virtual BWString GetInnerHTML() { return htmlTableGetInnerHTML(); }
    BWAjaxHtmlTable& SetTemplateVar(const BWString &var, const BWString &value);

protected:
    //void addDisplayField(const BWString &field);
    void reset();
    //void addDataField(const BWString &field, const BWList &lst);
    //void setDisplayFieldTemplate(const BWString &field, const BWString &fieldtemplate);
    //void setDisplayFieldType(const BWString &field, const BWString &type);
    BWString htmlTableGetInnerHTML();

private:
    void generateTemplate();
    BWString getFieldTemplate(const BWString &field);
    BWString getOptionsTemplate(const BWString &field);
    void engine();

protected:
    BWKeyPair m_data;
    BWConfig m_config;
    BWHttp* m_http;
    BWKeyList m_datafields;
    BWKeyPair m_fields;
    BWKeyPair m_template_vars;
    BWTemplate m_template;
    BWString m_nrpages;
    BWLog m_log;
};

}

#endif
