//
// C++ Interface: bwhtmltable
//
// Description: 
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWHTMLTABLE_H
#define BWHTMLTABLE_H

#include <bwconfig.h>
#include <bwkeylist.h>
#include <bwhttp.h>
#include <bwtemplate.h>
#include <bwlist.h>
#include <bwerror.h>

/**
Classe responsavel pela formatacao de tabelas HTML

	@author Bruno Moreira Guedes
*/

using namespace std;
using namespace Http;

namespace CGI {

class BWHtmlTable{
public:
    BWHtmlTable(const BWString &configfile, BWHttp *http);
    BWHtmlTable(BWConfig &config, BWHttp *http);

    virtual ~BWHtmlTable() {}
    void SetChannel(BWHttp *http);
    virtual BWString GetInnerHTML() { return htmlTableGetInnerHTML(); }
    BWHtmlTable& Send();
    BWHtmlTable& SetTemplateVar(const BWString &var, const BWString &value);

protected:
    BWConfig bw_config;
    BWHttp* bw_http;
    BWKeyList tab_datafields;
    BWKeyPair tab_fields;
    BWKeyPair template_vars;
    BWTemplate tab_template;
private:
    void generateTemplate();
    BWString getFieldTemplate(const BWString &field);
    BWString loadTemplateFromFile(const BWString &file);
    void engine();
    void init(BWConfig &cfg, BWHttp *http);
protected:
    void addDisplayField(const BWString &field);
    void reset();
    void addDataField(const BWString &field, const BWList &lst);
    void setDisplayFieldTemplate(const BWString &field, const BWString &fieldtemplate);
    void setDisplayFieldType(const BWString &field, const BWString &type);
    BWString htmlTableGetInnerHTML();
};

}

#endif
