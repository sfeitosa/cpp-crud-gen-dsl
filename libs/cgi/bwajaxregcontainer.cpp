//
// C++ Implementation: bwajaxregcontainer
//
// Description: 
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2013
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwajaxregcontainer.h"

using namespace CGI;

namespace CGI {

BWAjaxRegContainer::BWAjaxRegContainer(BWKeyPair &data,
		const BWString &crud, BWDb &db) {
	m_data = data;
	m_crud.Load(crud);
	m_db = &db;

	preParsing();
}

BWAjaxRegContainer::~BWAjaxRegContainer() {
}

}

/*!
 \fn CGI::BWRegContainer::preParsing()
 */
void CGI::BWAjaxRegContainer::preParsing() {
	BWList datafields;
	unsigned int i;
	BWString label;

	if (m_crud["datafields"] == "(null)") {
		throw BWFatalError(CGI_ERROR, "Sem datafields definido.");
	}
	datafields = m_crud["datafields"].Split(",");

	m_crud["datafields"] = datafields.ToString();

	for (i = 0; i < datafields.size(); i++) {
		label = m_crud[datafields[i] + ".label"];
		if (label != "(null)") {
			m_crud[datafields[i] + ".fieldlabel"] = label;
			m_crud[datafields[i] + ".columnlabel"] = label;
		}
	}

	m_datafields = datafields;

	if (m_crud["prefix"] == "(null)") {
		m_tablename = m_crud["dbtable"];
	} else {
		m_tablename = m_crud["prefix"] + "_" + m_crud["dbtable"];
	}
}

BWString CGI::BWAjaxRegContainer::generateMessage(const BWString &type, const BWString &msg) {
	BWTemplate alert;
	BWString classtype = "alert-" + type;

	alert.FromFile(_BW_HTML_ALERT);
	alert.SetAttribute("ALERT_CLASS", classtype);
	alert.SetAttribute("MESSAGE", msg);

	return alert.GetText();
}

/*!
    \fn CGI::BWRegContainer::GetInnerHtml(const BWString &action)
 */
BWString CGI::BWAjaxRegContainer::GetInnerHtml() {

	m_log(BWLog::MSG_XDEBUG) << "BWAjaxRegContainer::GetInnerHtml" << bwlog::endl;

	return executeAction(m_data["__action_name"]);
}


/*!
    \fn CGI::BWRegContainer::doAction(const BWString &action)
 */
BWString CGI::BWAjaxRegContainer::executeAction(const BWString &action) {
    BWString html;
    BWAccessProfile prof(m_data["BWC_USER_ID"].GetInt(), m_data["cad"], m_db);

	m_log(BWLog::MSG_XDEBUG) << "BWAjaxRegContainer::executeAction -> " << action << bwlog::endl;

    if (!prof.CanAccess()) {
        html = generateMessage("danger bw-error-msg",
                          "Sem permissão de acesso ao formulário.");
        return html;
    }

	if (action == "create" || action == "edit") {
	    if (action == "create" && !prof.CanCreate()) {
	        html = generateMessage("danger bw-error-msg",
	                               "Sem permissão para criação de registro.");
	    }
	    else if (action == "edit" && prof.CanEdit() == "N") {
	        html = generateMessage("danger bw-error-msg",
	                                           "Sem permissão para edição de registro.");
	    }
	    else {
		    html = doEdit(action);
	    }
	} else if (action == "delete") {
	    if (prof.CanDelete() != "N") {
		    html = doDelete();
	    }
	    else {
	        html = generateMessage("danger bw-error-msg",
	                               "Sem permissão para exclusão de registro.");
	    }
	} else if (action == "moveup") {
		html = doMoveUp();
	} else if (action == "movedown") {
		html = doMoveDown();
	} else if (action == "disable") {
		//doDisable();
	} else if (action == "enable") {
		//doEnable();
	} else {
	    if (prof.CanList() != "N") {
		    html = doList();
	    }
	    else {
	        html = generateMessage("danger bw-error-msg",
	                               "Sem permissão de visualização.");
	    }
	}

	m_log(BWLog::MSG_XDEBUG) << "BWAjaxRegContainer::executeAction -> Generated HTML: \n" << html << bwlog::endl;

	return html;
}

/*!
    \fn CGI::BWRegContainer::doEdit()
 */
BWString CGI::BWAjaxRegContainer::doEdit(const BWString &action) {
	BWString html;
	BWAccessProfile prof(m_data["BWC_USER_ID"].GetInt(), m_data["cad"], m_db);

	m_log(BWLog::MSG_XDEBUG) << "BWAjaxRegContainer::doEdit" << bwlog::endl;

	if (action == "edit" && !matchCreator(m_data["form_id__"])) {
	    if (prof.CanEdit() != "T") {
	        html = generateMessage("danger bw-error-msg", "Sem permissão para editar registro!");
	        return html;
	    }
	}

    preForm(action);

	try {
		m_form = new BWAjaxDbHtmlForm(m_data, m_crud, *m_db);
	} catch (BWError &e) {
		e.TrackError(m_BWErrorPlace, "Erro ao carregar novo form");
		throw e;
	}

	if (m_form->WasSubmitted()) {

		m_log(BWLog::MSG_XDEBUG) << "BWAjaxRegContainer::doEdit -> form was submitted" << bwlog::endl;

		try {
		    m_db->BeginTransaction();

			m_data["registry_id"] = m_form->Save();
			// do post actions
			doFormAction("postsave");
            // write information to keepsync
			setLastUpdate();
			m_db->Commit();
		}
		catch (BWError &e) {
		    if (m_db->AtTransaction()) {
		        m_db->Rollback();
		    }

			html = generateMessage("danger bw-error-msg",
			      "Erro: " + e.GetErrorMessage());
			return html;
		}

		html = doList(generateMessage("success", "Informação cadastrada com sucesso!"));
	}
	else {
		m_log(BWLog::MSG_XDEBUG) << "BWAjaxRegContainer::doEdit -> form wasn't submitted" << bwlog::endl;

	    html = m_form->GetInnerHtml();
	}

	delete m_form;

	return html;
}

/*!
    \fn CGI::BWRegContainer::doDelete()
 */
BWString CGI::BWAjaxRegContainer::doDelete() {
    BWString html;
    BWKeyPair rec;
    BWKeyPair where;
    BWAccessProfile prof(m_data["BWC_USER_ID"].GetInt(), m_data["cad"], m_db);

    if (!matchCreator(m_data["form_id__"])) {
        if (prof.CanDelete() != "T") {
            html = generateMessage("danger bw-error-msg", "Sem permissões para excluir registro!");
            return doList(html);
        }
    }

    rec["ativo"] = "N";
    where["id"] = m_data["form_id__"];

	try {
		m_db->Update(m_tablename, rec, where);
		setLastUpdate();
	} catch (BWError &e) {
		html = generateMessage("danger bw-error-msg",
				"Erro ao excluir registro: " + e.GetErrorMessage());
	}

	html = generateMessage("success", "Registro removido com sucesso!");

	return doList(html);
}

/*!
    \fn CGI::BWRegContainer::doList()
 */
BWString CGI::BWAjaxRegContainer::doList(const BWString &msg) {
	BWTemplate panel;
	BWString info;

	panel.FromFile(_BW_HTML_PANEL);

	info  = "<div id='bw-dialog-message' class='fade'>" + msg + "</div>\n";
	info += m_crud["info"];

	panel.SetAttribute("TITLE", m_crud["label"]);
	panel.SetAttribute("INFO", info);
	panel.SetAttribute("OPTIONS", getOptions(m_crud["options"]));

    try {
       m_table = new BWAjaxDbHtmlTable(m_data, m_crud, *m_db);
       panel.SetAttribute("CONTENT", m_table->GetInnerHTML());
    }
    catch (BWError &e) {
       e.TrackError(m_BWErrorPlace,
                    "Erro ao criad DbHtmlTable");
       throw e;
    }

    delete m_table;
    return panel.GetText();
}


/*!
    \fn CGI::BWAjaxRegContainer::getOptions(const BWString &options)
 */
BWString CGI::BWAjaxRegContainer::getOptions(const BWString &options) {
	BWList opt;
	BWTemplate button;
	BWString query;
	BWString html = "";
	BWString descr = "";
	BWAccessProfile prof(m_data["BWC_USER_ID"].GetInt(), m_data["cad"], m_db);

	if (options == "" || !prof.CanCreate()) {
		return html;
	}

	button.FromFile(_BW_HTML_BUTTON);

	opt = options.Split(",");

	for (unsigned int i = 0; i < opt.size(); i++) {
		query  = "?cad=";
		query += m_data["cad"];
		query += "&__action_name=";
		query += opt[i];

		button.SetAttribute("HREF", query);

		if (m_crud["options." + opt[i] + ".descr"] != "(null)") {
			descr = m_crud["options." + opt[i] + ".descr"];
			button.SetAttribute("TEXT", descr);
		}

		if (opt[i] == "create") {
			button.SetAttribute("CLASS", "btn-primary bwmodal");
			if (descr == "") {
			    button.SetAttribute("TEXT", "<span class='glyphicon glyphicon-plus'></span> Novo Registro");
			}
		}
		else {
			button.SetAttribute("CLASS", "btn-default");
			if (descr == "") {
			    button.SetAttribute("TEXT", opt[i]);
			}
		}

		html += button.GetText();
	}

	return html;
}

/*!
    \fn CGI::BWRegContainer::doMoveDown()
 */
BWString CGI::BWAjaxRegContainer::doMoveDown() {
	BWString query("SELECT (SELECT ");
	BWResultSet rs;
	BWString order = m_crud["orderfields"];
	BWString action;
	BWString table = m_tablename;
	BWKeyPair set_update_up, set_update_down, where_update_up,
			where_update_down, clear_order;
	BWString lastchange = "";
	BWString html;

	/*
	if (m_crud["keepsync"] == "true") {
		lastchange = ", ultimaalteracao = '";
		lastchange += get_date(BWMASK_DTNOW) + "'";
	}
	*/

	where_update_up["id"] = m_data["form_id__"];

	query += order + " FROM " + table + " WHERE id=" + where_update_up["id"];
	query += ") AS pr_up, id AS id_down, " + order + " AS pr_down FROM ";
	query += table + " WHERE " + order + "=(SELECT MIN(" + order + ") FROM ";
	query += table + " WHERE " + order + ">(SELECT " + order + " FROM " + table
			+ " WHERE id=";
	query += where_update_up["id"] + "))";

	rs = m_db->ExecuteQuery(query);

	where_update_down["id"] = rs.GetColumn("id_down")[0];
	set_update_up[order] = rs.GetColumn("pr_down")[0];
	set_update_down[order] = rs.GetColumn("pr_up")[0];

	where_update_down[order] = rs.GetColumn("id")[0];

	clear_order[order] = "0";

	try {
		m_db->BeginTransaction();
		m_db->ExecuteUpdate(
				BWString("UPDATE ") + table + " SET " + order + "="
						+ clear_order[order] + lastchange + " WHERE id="
						+ where_update_up["id"]);
		m_db->ExecuteUpdate(
				BWString("UPDATE ") + table + " SET " + order + "="
						+ set_update_down[order] + lastchange + " WHERE id="
						+ where_update_down["id"]);
		m_db->ExecuteUpdate(
				BWString("UPDATE ") + table + " SET " + order + "="
						+ set_update_up[order] + lastchange + " WHERE id="
						+ where_update_up["id"]);

		setLastUpdate();

		m_db->Commit();
	} catch (BWError &e) {
		m_db->Rollback();
		html = generateMessage("danger bw-error-msg", e.GetErrorMessage());
	}

	html = generateMessage("success", "Registro movido com sucesso!");

	return doList(html);
}


/*!
    \fn CGI::BWRegContainer::doMoveUp()
 */
BWString CGI::BWAjaxRegContainer::doMoveUp() {
	BWString query("SELECT (SELECT ");
	BWResultSet rs;
	BWString order = m_crud["orderfields"];
	BWString table = m_tablename;
	BWString action;
	BWKeyPair set_update_up, set_update_down, where_update_up,
			where_update_down, clear_order;
	BWString lastchange = "";
	BWString html;

	/*
	if (m_crud["keepsync"] == "true") {
		lastchange = ", ultimaalteracao = '";
		lastchange += get_date(BWMASK_DTNOW) + "'";
	}
	*/

	where_update_up["id"] = m_data["form_id__"];

	query += order + " FROM " + table + " WHERE id=" + where_update_up["id"];
	query += ") AS pr_up, id AS id_down, " + order + " AS pr_down FROM ";
	query += table + " WHERE " + order + "=(SELECT MAX(" + order + ") FROM ";
	query += table + " WHERE " + order + "<(SELECT " + order + " FROM " + table
			+ " WHERE id=";
	query += where_update_up["id"] + "))";

	rs = m_db->ExecuteQuery(query);

	where_update_down["id"] = rs.GetColumn("id_down")[0];
	set_update_up[order] = rs.GetColumn("pr_down")[0];
	set_update_down[order] = rs.GetColumn("pr_up")[0];

	where_update_down[order] = rs.GetColumn("id")[0];

	clear_order[order] = "0";
	try {
		m_db->BeginTransaction();
		m_db->ExecuteUpdate(
				BWString("UPDATE ") + table + " SET " + order + "="
						+ clear_order[order] + lastchange + " WHERE id="
						+ where_update_up["id"]);
		m_db->ExecuteUpdate(
				BWString("UPDATE ") + table + " SET " + order + "="
						+ set_update_down[order] + lastchange + " WHERE id="
						+ where_update_down["id"]);
		m_db->ExecuteUpdate(
				BWString("UPDATE ") + table + " SET " + order + "="
						+ set_update_up[order] + lastchange + " WHERE id="
						+ where_update_up["id"]);

		setLastUpdate();

		m_db->Commit();
	} catch (BWError &e) {
		m_db->Rollback();
		html = generateMessage("danger bw-error-msg", e.GetErrorMessage());
	}

	html = generateMessage("success", "Registro movido com sucesso!");

    return doList(html);
}


/*!
    \fn CGI::BWRegContainer::doDisable()
 */
/*void CGI::BWRegContainer::doDisable()
{
     BWKeyPair update_set, update_where;
     BWString action;

     update_set["ativo"] = "N";

     if (bw_config["keepsync"] == "true") {
         update_set["ultimaalteracao"] = get_date(BWMASK_DTNOW);
     }

     update_where["id"] = bw_http->GetParameter("__disable_id");

     bw_db->Update(rc_tablename, update_set, update_where);

     action = bw_http->GetServerVar("REQUEST_URL");
     action.Replace("&__action_name=disable", "");
     action.Replace(BWString("&__disable_id=") + bw_http->GetParameter("__disable_id"),
                    "");
     bw_http->SetHeader("Location", action);
}*/


/*!
    \fn CGI::BWRegContainer::preForm()
 */
void CGI::BWAjaxRegContainer::preForm(const BWString &action)
{
    unsigned int i, j;
    BWList formfields;
    BWString field;
    
	m_log(BWLog::MSG_XDEBUG) << "BWAjaxRegContainer::preForm" << bwlog::endl;

    if (m_crud["formfields"] == "(null)") {
        throw BWFatalError(CGI_ERROR,
                           "Parametro formfields obrigatorio.");
    }

    // Tratamento do criador do registro
    if (m_crud["handleprofile"] == "true" && m_data["BWC_USER_ID"] != "0") {
        m_crud["datafields"] += ",creator";
        m_crud["formfields"] += ",creator";
        m_crud["creator.fieldtype"] = "hidden";
        m_crud["creator.default"] = m_data["BWC_USER_ID"];
        m_crud["creator.dbfield"] = "creator";
    }

    formfields = m_crud["formfields"].Split(",");
    
    for (i = 0; i < formfields.size(); i++) {
         field = formfields[i];

         if (m_crud[field + ".dbfield"] == "(null)") {
             if (m_crud[field + ".relation"] == "(null)" ||
                 m_crud[field + ".multirelation"] == "(null)") {

                 for (j = 0; j < m_datafields.size(); j++) {
                      if (m_datafields[j] == field) {
                          m_crud[field + ".dbfield"] = m_datafields[j];
                          break;
                      }
                 }
             }
         }

         if (m_crud[field + ".label"] == "(null)") {
             m_crud[field + ".label"] = m_crud[field + ".fieldlabel"];
         }

         if (m_crud[field + ".fieldtype"] == "(null)") {
        	 if (action == "create") {
        		 if (m_crud[field + ".fieldtype.onCreate"] != "(null)") {
        			 m_crud[field + ".type"] = m_crud[field + ".fieldtype.onCreate"];
        		 }
        		 else {
        		     m_crud[field + ".type"] = "text";
        		 }
        	 }
        	 else {
            	 if (m_crud[field + ".fieldtype.onEdit"] != "(null)") {
            		 m_crud[field + ".type"] = m_crud[field + ".fieldtype.onEdit"];
            	 }
            	 else {
            		 m_crud[field + ".type"] = "text";
            	 }
        	 }
         }
         else {
             m_crud[field + ".type"] = m_crud[field + ".fieldtype"];
         }
    }

    if (m_crud["dbtable"] == "(null)") {
        throw BWFatalError(CGI_ERROR,
                           "Sem dbtable preenchido.");
    }
    
    m_crud["maintable"] = m_crud["dbtable"];
    m_crud["fields"] = formfields.ToString();
    m_crud["name"] = "form";
}

/*!
    \fn CGI::BWRegContainer::HasDataChange(void)
 */
/*
bool CGI::BWAjaxRegContainer::HasDataChange(void) {
	return has_data_change;
}
*/
/*
BWDbRelation CGI::BWAjaxRegContainer::getRelation(const BWString &field) {
    BWDbRelation rel(bw_db, bw_config["maintable"],
                     bw_config[field + ".multirelation"]);


    if (bw_config["keepsync"] == "true") {
        rel.SetKeepSync();
    }

    if (bw_config["prefix"] != "(null)" &&
        bw_config["prefix"] != "") {
        rel.SetPrefix(bw_config["prefix"]);
    }

    return rel;
}

*/

void CGI::BWAjaxRegContainer::doFormAction(const BWString &when) {
    BWFormAction *act = NULL;

    if (m_crud["formaction." + when + ".object"] != "(null)") {
        act = BWFormActionFactory::Create(m_crud["formaction." + when + ".object"], m_db);
        act->Exec(m_crud["formaction." + when + ".action"], m_data);
    }
}

bool CGI::BWAjaxRegContainer::matchCreator(const BWString &id) {
    BWString query;
    BWResultSet rset;

    // Usuário passado como zero para não tratar informações
    // de perfil
    if (m_data["BWC_USER_ID"] == "0") {
        return true;
    }

    query  = "SELECT id FROM " + m_tablename + " WHERE id = " + id;
    query += " AND creator = " + m_data["BWC_USER_ID"];

    rset = m_db->ExecuteQuery(query);

    if (rset.GetNumRows() > 0) {
        return true;
    }

    return false;
}

void CGI::BWAjaxRegContainer::setLastUpdate() {
    BWString cmd;

    if (m_crud["sync"] == "true") {
        cmd  = "UPDATE bw_ultimaalteracao SET datahora = '";
        cmd += get_date(BWMASK_DTNOW);
        cmd += "'";

        m_db->ExecuteUpdate(cmd);
    }
}
