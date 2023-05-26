//
// C++ Implementation: bwhtmltable
//
// Description: 
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwhtmltable.h"
#include <fstream>

using namespace CGI;
using namespace std;

BWHtmlTable::BWHtmlTable(const BWString &configfile, BWHttp *http)
{
    BWConfig cfg;
    cfg.Load(configfile);
    
    init(cfg, http);
}

BWHtmlTable::BWHtmlTable(BWConfig &config, BWHttp *http)
{
    init(config, http);
}

/*!
    \fn CGI::BWHtmlTable::generateTemplate()
 */
void CGI::BWHtmlTable::generateTemplate()
{
    BWString pretpl("<TABLE class='table_view'>\n <CAPTION>"), 
             templatestr1(" </CAPTION>\n <THEAD>\n  <TR>\n"), 
             templatestr2("  </TR>\n </THEAD>\n <TBODY>\n  <%!foreach <TR class='table_body'>\n"),
             endtpl("  </TR>\n  !%>\n </TBODY>\n</TABLE>"),
             templatestr;
    BWKeyPair::iterator itr = tab_fields.begin();
    BWKeyList::iterator itrl = tab_datafields.begin();

    // Adicionado por Samuel em 28/10/2011 para mostrar os campos
    // da tabela na ordem inserida no arquivo de configuracao
    BWList lst = bw_config["tablefields"].Split(",");

    for (unsigned int i = 0; i < lst.size(); i++) {
        templatestr1 += "   <TD class='table_head'>" + bw_config[lst[i] + ".label"] + "</TD>\n";
        templatestr2 += "   <TD>" + tab_fields[lst[i]] + "</TD>\n";
    }

    // Fim da alteracao em substituicao do codigo comentado abaixo

    /*
    while (itr != tab_fields.end()) {
                templatestr1 += "   <TD class='table_head'>" + bw_config[itr->first + ".label"] + "</TD>\n";
                templatestr2 += "   <TD>" + itr->second + "</TD>\n";
                itr++;
    }
    */

    templatestr = pretpl + bw_config["label"] + templatestr1 + templatestr2 + endtpl;
    tab_template.SetText(templatestr);

    while (itrl != tab_datafields.end()) {
                tab_template.SetAttributeList(itrl->first, itrl->second);
                itrl++;
    }
    
    itr = template_vars.begin();
    while (itr != template_vars.end()) {
                tab_template.SetAttribute(itr->first, itr->second);
                itr++;
    }
    
    tab_template.SetAttributes(bw_config);

}


/*!
    \fn CGI::BWHtmlTable::htmlTableGetInnerHTML()
 */
BWString CGI::BWHtmlTable::htmlTableGetInnerHTML()
{
    engine();
    return tab_template.GetText();
}


/*!
    \fn CGI::BWHtmlTable::getFieldTemplate(const BWString &field)
 */
BWString CGI::BWHtmlTable::getFieldTemplate(const BWString &field)
{
    BWKeyList::iterator itr = tab_datafields.begin();
    BWString ret("<%#");
    BWString openf = BW_ROOT_DIR;
    openf += "etc/celltypes/";

    if (bw_config[field + ".type"] == "" || bw_config[field + ".type"] == "(null)") {
        if (bw_config[field + ".template"] == "" || bw_config[field + ".template"] == "(null)") {
            while(itr != tab_datafields.end()) {
                  if (field == itr->first) {
                      break;
                  }
                  itr++;
            }

            if (itr != tab_datafields.end()) {
                return (ret + field + "#%>");
            }
            else {
                return "";
            }
        }
        else {
            return bw_config[field + ".template"];
        }
    }
    else {
        return loadTemplateFromFile(openf + bw_config[field + ".type"] + ".tct"); 
    }
}


/*!
    \fn CGI::BWHtmlTable::SetChannel(BWHttp *http)
 */
void CGI::BWHtmlTable::SetChannel(BWHttp *http)
{
    if (http) {
        bw_http = http;
    }
    else {
        throw BWFatalError(CGI_ERROR,
                           "Recurso manipulador de HTTP Invalido/Nulo.");
    }
}


/*!
    \fn CGI::BWHtmlTable::loadTemplateFromFile(const BWString &file)
 */
BWString CGI::BWHtmlTable::loadTemplateFromFile(const BWString &file)
{
    BWString fbuffer, lnbuffer;
    fstream tplfile(file.c_str(), fstream::in);

    while (getline(tplfile, lnbuffer)) {
                fbuffer += lnbuffer + "\n";
    }

    return fbuffer;
}


/*!
    \fn CGI::BWHtmlTable::engine()
 */
void CGI::BWHtmlTable::engine()
{
    BWList lst;
    unsigned int i;
    BWString tablefields = bw_config["tablefields"], 
             datafields = bw_config["datafields"];

    if (tablefields == "" ||
        tablefields == "(null)" ||
        datafields == "" ||
        datafields == "(null)")
    {
        throw BWFatalError(CGI_ERROR,
                           "Arquivo de configuracao incompleto ou inexistente.");
    }

    lst = datafields.Split(",");
    for (i = 0; i < lst.size(); i++) {
                if (bw_config[lst[i] + ".values"] == "(null)") {
                            continue;
                }
                if (bw_config[lst[i] + ".ids"] == "(null)" ||
                    bw_config[lst[i] + ".descrs"] == "(null)")
                {
                    tab_datafields[lst[i]] = bw_config[lst[i] + ".values"].Split(",");
                }
                else {
                    BWList    ids = bw_config[lst[i] + ".ids"].Split(","),
                           descrs = bw_config[lst[i] + ".descrs"].Split(",");
                    BWString v = ",", values=bw_config[lst[i] + ".values"];

                    for (unsigned int j = 0;
                         j < ids.size(); j++) 
                    {
                         if (values == ids[j]) {
                             values = descrs[j];
                         }
                         else {
                             values.Replace(v + ids[j], v + descrs[j]);
                             values.Replace(ids[j] + v, descrs[j] + v);
                             tab_datafields[lst[i]] = values.Split(",");
                         }
                    }
                }
    }

    lst = tablefields.Split(",");
    for (i = 0; i < lst.size(); i++) {
         tab_fields[lst[i]] = getFieldTemplate(lst[i]);
    }

    generateTemplate();
}


/*!
    \fn CGI::BWHtmlTable::addDisplayField(const BWString &field)
 */
void CGI::BWHtmlTable::addDisplayField(const BWString &field)
{
    if (bw_config["tablefields"] != "(null)" && bw_config["tablefields"] != "") {
        bw_config["tablefields"] += "," + field;
    }
    else {
        bw_config["tablefields"] = field;
    }
    
    tab_fields[field] = "";
}


/*!
    \fn CGI::BWHtmlTable::reset()
 */
void CGI::BWHtmlTable::reset()
{
    tab_fields = BWKeyPair();
    tab_datafields = BWKeyList();
    tab_template = BWTemplate();
}


/*!
    \fn CGI::BWHtmlTable::Send()
 */
BWHtmlTable& CGI::BWHtmlTable::Send()
{
    *bw_http << GetInnerHTML();
    
    return *this;
}


/*!
    \fn CGI::BWHtmlTable::addDataField(const BWString &field, const BWList &lst)
 */
void CGI::BWHtmlTable::addDataField(const BWString &field, const BWList &lst)
{
    tab_datafields[field] = lst;
}


/*!
    \fn CGI::BWHtmlTable::setDisplayFieldTemplate(const BWString &field, const BWString &fieldtemplate)
 */
void CGI::BWHtmlTable::setDisplayFieldTemplate(const BWString &field, const BWString &fieldtemplate)
{
    if (tab_fields[field] == "(null)" || tab_fields[field] == "") {
        bw_config[field + ".template"] = fieldtemplate;
        tab_fields[field] = fieldtemplate;
    }
}


/*!
    \fn CGI::BWHtmlTable::setDisplayFieldType(const BWString &field, const BWString &type)
 */
void CGI::BWHtmlTable::setDisplayFieldType(const BWString &field, const BWString &type)
{
    if (tab_fields[field] == "(null)" || tab_fields[field] == "") {
        bw_config[field + ".type"] = type;
        tab_fields[field] = loadTemplateFromFile(BWString(BW_ROOT_DIR) + 
                                                 "etc/celltypes/" + 
                                                 bw_config[field + ".type"] +
                                                 ".tct");
    }
}


/*!
    \fn CGI::BWHtmlTable::SetTemplateVar(const BWString &var, const BWString &value)
 */
BWHtmlTable& CGI::BWHtmlTable::SetTemplateVar(const BWString &var, const BWString &value)
{
    template_vars[var] = value;
    
    return *this;
}


/*!
    \fn CGI::BWHtmlTable::init(BWConfig &cfg, BWHttp *http)
 */
void CGI::BWHtmlTable::init(BWConfig &cfg, BWHttp *http)
{
     bw_config = cfg;
     try {
           SetChannel(http);
     }
     catch(BWError &e) {
           e.TrackError(m_BWErrorPlace,
                        "Erro definindo canal HTTP");
           if (e.IsFatal()) {
               throw e;
           }
      }
      
      SetTemplateVar("REQUEST_URI", bw_http->GetServerVar("REQUEST_URI"));
      SetTemplateVar("REQUEST_URL", bw_http->GetServerVar("REQUEST_URL"));
}
