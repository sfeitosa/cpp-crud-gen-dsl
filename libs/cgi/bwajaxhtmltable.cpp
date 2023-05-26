#include "bwajaxhtmltable.h"
#include <fstream>

using namespace CGI;
using namespace std;

BWAjaxHtmlTable::BWAjaxHtmlTable(BWKeyPair &data, const BWString &cfg)
{
	m_data = data;
    m_config.Load(cfg);
    
}

BWAjaxHtmlTable::BWAjaxHtmlTable(BWKeyPair &data, BWConfig &cfg)
{
	m_data = data;
    m_config = cfg;
}

/*!
    \fn CGI::BWAjaxHtmlTable::generateTemplate()
 */
void CGI::BWAjaxHtmlTable::generateTemplate()
{
	BWList lst = m_config["tablefields"].Split(",");
	BWKeyPair::iterator itr;
	BWKeyList::iterator itrl = m_datafields.begin();
	BWString header = "";
	BWString body = "";

	m_template.FromFile(_BW_HTML_TABLE);

    for (unsigned int i = 0; i < lst.size(); i++) {
        header += "<th>" + m_config[lst[i] + ".label"] + "</th>";
        body += "<td>" + m_fields[lst[i]] + "</td>";
    }

    if (m_config["tableoptions"] != "(null)" && m_config["tableoptions"] != "") {
    	header += "<th>Opções</th>";
    	body += "<td>" + m_fields["options"] + "</td>";
    }

    if (m_config["table.reloadtime"] != "(null)" && m_config["table.reloadtime"] != "") {
        m_template.SetAttribute("RELOAD_TIME", m_config["table.reloadtime"]);
    }
    else {
        m_template.SetAttribute("RELOAD_TIME", "0");
    }

    if (m_data["curpage"] == "(null)") {
    	m_template.SetAttribute("CUR_PAGE", "1");
    }
    else {
        m_template.SetAttribute("CUR_PAGE", m_data["curpage"]);
    }
    m_template.SetAttribute("NR_PAGES", m_nrpages);
    m_template.SetAttribute("HEADER", header);
    m_template.ReplaceText("BODY", body);

    while (itrl != m_datafields.end()) {
                m_template.SetAttributeList(itrl->first, itrl->second);
                itrl++;
    }
    
    itr = m_template_vars.begin();
    while (itr != m_template_vars.end()) {
                m_template.SetAttribute(itr->first, itr->second);
                itr++;
    }
    
    m_template.SetAttributes(m_config);
}


/*!
    \fn CGI::BWAjaxHtmlTable::htmlTableGetInnerHTML()
 */
BWString CGI::BWAjaxHtmlTable::htmlTableGetInnerHTML()
{
    engine();
    return m_template.GetText();
}


/*!
    \fn CGI::BWAjaxHtmlTable::getFieldTemplate(const BWString &field)
 */
BWString CGI::BWAjaxHtmlTable::getFieldTemplate(const BWString &field)
{
	BWKeyList::iterator itr = m_datafields.find(field);
	BWString templ;

	if (itr != m_datafields.end()) {
		templ = "<%#" + itr->first + "#%>";
	    return templ;
	}

    return "";
}

/*!
    \fn CGI::BWAjaxHtmlTable::getOptionsTemplate(const BWString &field)
 */
BWString CGI::BWAjaxHtmlTable::getOptionsTemplate(const BWString &option)
{
	BWTemplate button;
	BWString query;
	BWString descr = "";

	button.FromFile(_BW_HTML_BUTTON);

	query  = "?cad=";
	query += m_data["cad"];
	query += "&__action_name=";
	query += option;
	query += "&form_id__=<%#id#%>";

	button.SetAttribute("HREF", query);

	if (m_config["tableoptions." + option + ".descr"] != "(null)") {
		descr = m_config["tableoptions." + option + ".descr"];
		button.SetAttribute("TEXT", descr);
	}

	if (option == "edit") {
		button.SetAttribute("CLASS", "btn-success btn-sm bwmodal");
		if (descr == "") {
		    button.SetAttribute("TEXT", "<span class='glyphicon glyphicon-pencil'></span> Editar");
		}
	} else if (option == "delete") {
		button.SetAttribute("CLASS", "btn-danger btn-sm bwoption");
		if (descr == "") {
		    button.SetAttribute("TEXT", "<span class='glyphicon glyphicon-trash'></span> Excluir");
		}
	} else if (option == "moveup") {
		button.SetAttribute("CLASS", "btn-warning btn-sm bwoption");
		if (descr == "") {
		    button.SetAttribute("TEXT", "<span class='glyphicon glyphicon-arrow-up'></span> Mover");
		}
    } else if (option == "movedown") {
		button.SetAttribute("CLASS", "btn-warning btn-sm bwoption");
		if (descr == "") {
		    button.SetAttribute("TEXT", "<span class='glyphicon glyphicon-arrow-down'></span>Mover");
		}
	}
    else {
    	button.SetAttribute("CLASS", "btn-default btn-sm bwoption");
    	if (descr == "") {
		    button.SetAttribute("TEXT", option);
    	}
    }

	return button.GetText();
}

/*!
    \fn CGI::BWAjaxHtmlTable::engine()
 */
void CGI::BWAjaxHtmlTable::engine()
{
    BWList lst;
    unsigned int i;
    BWString tablefields = m_config["tablefields"];
    BWString tableoptions = m_config["tableoptions"];
    BWString datafields = m_config["datafields"];

    if (tablefields == "(null)" || datafields == "(null)")
    {
        throw BWFatalError(CGI_ERROR,
                           "Arquivo de configuracao incompleto ou inexistente.");
    }

    lst = datafields.Split(",");
	for (i = 0; i < lst.size(); i++) {
		if (m_config[lst[i] + ".values"] == "(null)" ||
			m_config[lst[i] + ".values"] == "") {
			continue;
		}
		if (m_config[lst[i] + ".ids"] == "(null)" ||
			m_config[lst[i] + ".ids"] == "" ||
			m_config[lst[i] + ".descrs"] == "(null)" ||
			m_config[lst[i] + ".descrs"] == "") {

			m_datafields[lst[i]] = m_config[lst[i] + ".values"].Split(",");

		} else {
			BWList ids = m_config[lst[i] + ".ids"].Split(",");
			BWList descrs = m_config[lst[i] + ".descrs"].Split(",");
			BWString v = ",";
			BWString values = m_config[lst[i] + ".values"];

			for (unsigned int j = 0; j < ids.size(); j++) {
				if (values == ids[j]) {
					values = descrs[j];
				} else {
					values.Replace(v + ids[j], v + descrs[j]);
					values.Replace(ids[j] + v, descrs[j] + v);
					m_datafields[lst[i]] = values.Split(",");
				}
			}
		}
	}

    lst = tablefields.Split(",");
    for (i = 0; i < lst.size(); i++) {
         m_fields[lst[i]] = getFieldTemplate(lst[i]);
    }

    lst = tableoptions.Split(",");
    tableoptions = "";
    for (i = 0; i < lst.size(); i++) {
        tableoptions += getOptionsTemplate(lst[i]);
    }
    m_fields["options"] = tableoptions;

    generateTemplate();
}


/*!
    \fn CGI::BWAjaxHtmlTable::addDisplayField(const BWString &field)
 */
/*void CGI::BWAjaxHtmlTable::addDisplayField(const BWString &field)
{
    if (m_config["tablefields"] != "(null)" && m_config["tablefields"] != "") {
        m_config["tablefields"] += "," + field;
    }
    else {
        m_config["tablefields"] = field;
    }
    
    m_fields[field] = "";
}*/


/*!
    \fn CGI::BWAjaxHtmlTable::reset()
 */
void CGI::BWAjaxHtmlTable::reset()
{
    m_fields = BWKeyPair();
    m_datafields = BWKeyList();
    m_template = BWTemplate();
}


/*!
    \fn CGI::BWAjaxHtmlTable::addDataField(const BWString &field, const BWList &lst)
 */
/*void CGI::BWAjaxHtmlTable::addDataField(const BWString &field, const BWList &lst)
{
    m_datafields[field] = lst;
}*/


/*!
    \fn CGI::BWAjaxHtmlTable::setDisplayFieldTemplate(const BWString &field, const BWString &fieldtemplate)
 */
/*void CGI::BWAjaxHtmlTable::setDisplayFieldTemplate(const BWString &field, const BWString &fieldtemplate)
{
    if (m_fields[field] == "(null)" || m_fields[field] == "") {
        m_config[field + ".template"] = fieldtemplate;
        m_fields[field] = fieldtemplate;
    }
}*/


/*!
    \fn CGI::BWAjaxHtmlTable::setDisplayFieldType(const BWString &field, const BWString &type)
 */
/*void CGI::BWAjaxHtmlTable::setDisplayFieldType(const BWString &field, const BWString &type)
{
    if (m_fields[field] == "(null)" || m_fields[field] == "") {
        m_config[field + ".type"] = type;
        m_fields[field] = loadTemplateFromFile(BWString(BW_ROOT_DIR) +
                                                 "etc/celltypes/" + 
                                                 m_config[field + ".type"] +
                                                 ".tct");
    }
}*/


/*!
    \fn CGI::BWAjaxHtmlTable::SetTemplateVar(const BWString &var, const BWString &value)
 */
BWAjaxHtmlTable& CGI::BWAjaxHtmlTable::SetTemplateVar(const BWString &var, const BWString &value)
{
    m_template_vars[var] = value;
    
    return *this;
}


/*!
    \fn CGI::BWAjaxHtmlTable::init(BWConfig &cfg, BWHttp *http)
 */
/*void CGI::BWAjaxHtmlTable::init(BWConfig &cfg, BWHttp *http)
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
}*/
