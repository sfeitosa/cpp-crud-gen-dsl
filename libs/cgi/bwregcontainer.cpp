//
// C++ Implementation: bwregcontainer
//
// Description: 
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwregcontainer.h"

using namespace CGI;

namespace CGI {

BWRegContainer::BWRegContainer(const BWString &cfg, BWHttp *http, BWDb &db) {
	cerr << "Iniciando BWRegContainer" << endl;
	bw_config.Load(cfg);
	SetChannel(http);
	bw_db = &db;
	preParsing();
}

BWRegContainer::BWRegContainer(BWConfig &cfg, BWHttp *http, BWDb &db) {
	cerr << "Iniciando BWRegContainer 2" << endl;
	bw_config = cfg;
	SetChannel(http);
	bw_db = &db;
	preParsing();
}

BWRegContainer::~BWRegContainer() {
}

/*!
 \fn CGI::BWRegContainer::SetChannel(BWHttp *http)
 */
void CGI::BWRegContainer::SetChannel(BWHttp *http) {
	if (http) {
		bw_http = http;
	} else {
		throw BWFatalError(CGI_ERROR,
				"Recurso manipulador de HTTP Invalido/Nulo.");
	}
}

}

/*!
 \fn CGI::BWRegContainer::preParsing()
 */
void CGI::BWRegContainer::preParsing() {
	BWList datafields, actions;
	unsigned int i;
	bool hasaction = false;
	BWString label, action;

	cerr << "preParsing" << endl;

	if (bw_config["datafields"] == "(null)") {
		throw BWFatalError(CGI_ERROR, "Sem datafields definido.");
	}
	datafields = bw_config["datafields"].Split(",");

	bw_config["datafields"] = datafields.ToString();

	for (i = 0; i < datafields.size(); i++) {
		label = bw_config[datafields[i] + ".label"];
		if (label != "(null)") {
			bw_config[datafields[i] + ".fieldlabel"] = label;
			bw_config[datafields[i] + ".columnlabel"] = label;
		}
	}

	rc_datafields = datafields;

	action = bw_http->GetParameter("__action_name");
	if (action == "(null)") {
		action = "list";
	}

	cerr << "Action: " << action << endl;

	actions = bw_config["actions"].Split(",");

	for (i = 0; i < actions.size(); i++) {
		if (action == actions[i]) {
			hasaction = true;
			break;
		}
	}

	if (!hasaction) {
		action = "list";
	}

	if (bw_config["prefix"] == "(null)") {
		rc_tablename = bw_config["dbtable"];
	} else {
		rc_tablename = bw_config["prefix"] + "_" + bw_config["dbtable"];
	}

	doAction(action);
}

/*!
 \fn CGI::BWRegContainer::doAction(const BWString &action)
 */
void CGI::BWRegContainer::doAction(const BWString &action) {
	has_data_change = true;
	if (action == "create" || action == "edit") {
		if (bw_http->GetServerVar("REQUEST_METHOD") != "POST") {
			has_data_change = false;
		}
		doEdit();
	} else if (action == "delete") {
		doDelete();
	} else if (action == "moveup") {
		doMoveUp();
	} else if (action == "movedown") {
		doMoveDown();
	} else if (action == "disable") {
		doDisable();
	} else if (action == "enable") {
		doEnable();
	} else {
		has_data_change = false;
		doList();
	}
}

using namespace CGI;

/*!
 \fn CGI::BWRegContainer::doEdit()
 */
void CGI::BWRegContainer::doEdit() {
	BWString action;
	preForm();
	bw_config.SaveDump("/tmp/dump.out");
	try {
		rc_form = new BWDbHtmlForm(bw_config, bw_http, *bw_db);
	} catch (BWError &e) {
		e.TrackError(m_BWErrorPlace, "Erro ao carregar novo form");
		throw e;
	}

	if (rc_form->IsFilled()) {
		try {
			rc_form->Save();
		} catch (BWError &e) {
			e.TrackError(m_BWErrorPlace, "Erro ao salvar formulario.");
		}

		action = bw_http->GetServerVar("REQUEST_URL");
		action.Replace("&__action_name=create", "");
		action.Replace("&__action_name=edit", "");
		action.Replace(
				BWString("&form_id__=") + bw_http->GetParameter("form_id__"),
				"");
		bw_http->SetHeader("Location", action);
	} else {
		rc_form->Send();
	}

	delete rc_form;
}

/*!
 \fn CGI::BWRegContainer::doDelete()
 */
void CGI::BWRegContainer::doDelete() {
	BWKeyPair delete_where;
	BWString action, del_id = bw_http->GetParameter("__delete_id");
	BWList formfields, dbfields, multirelations;
	unsigned int i;

	preForm();

	BWDbRegistry dbreg(*bw_db, bw_config["maintable"]);

	cerr << "Iniciando doDelete: maintable -> " << bw_config["maintable"]
			<< endl;

	formfields = bw_config["fields"].Split(",");

	for (i = 0; i < formfields.size(); i++) {
		if (bw_config[formfields[i] + ".multirelation"] != "(null)") {
//             multirelations.push_back(formfields[i]);
			cerr << "doDelete: Adicionando relacao" << endl;
			dbreg.AddRelation(formfields[i], getRelation(formfields[i]));
			continue;
		} else if (bw_config[formfields[i] + ".relation"] != "(null)"
				|| bw_config[formfields[i] + ".dbfield"] != "(null)") {
			cerr << "Adicionando campo " << formfields[i] << endl;
//             dbfields.push_back(formfields[i]);
			dbreg.AddField(formfields[i]);
			continue;
		}
	}

	try {
		if (bw_config["keepsync"] == "true") {
			dbreg.SetKeepSync();
		}
		dbreg.SetTablePrefix(bw_config["prefix"]);

		cerr << "doDelete: del_id: " << del_id << endl;
		dbreg.Load(del_id.GetInt());
		dbreg.Delete();
		cerr << "Feito!" << endl;
	} catch (BWError &e) {
		e.TrackError(m_BWErrorPlace, "Erro ao deletar registro");
		cerr << "Caiu no catch: " << e.GetErrorMessage() << endl;
	}

	action = bw_http->GetServerVar("REQUEST_URL");
	action.Replace("&__action_name=delete", "");
	action.Replace(BWString("&__delete_id=") + del_id, "");
	bw_http->SetHeader("Location", action);
	cerr << "Finalizando doDelete" << endl;
}

/*!
 \fn CGI::BWRegContainer::doEnable()
 */
void CGI::BWRegContainer::doEnable() {
	BWKeyPair update_set, update_where;
	BWString action;

	update_set["ativo"] = "S";

	if (bw_config["keepsync"] == "true") {
		update_set["ultimaalteracao"] = get_date(BWMASK_DTNOW);
	}

	update_where["id"] = bw_http->GetParameter("__enable_id");

	bw_db->Update(rc_tablename, update_set, update_where);

	action = bw_http->GetServerVar("REQUEST_URL");
	action.Replace("&__action_name=enable", "");
	action.Replace(
			BWString("&__enable_id=") + bw_http->GetParameter("__enable_id"),
			"");
	bw_http->SetHeader("Location", action);
}

/*!
 \fn CGI::BWRegContainer::doList()
 */
void CGI::BWRegContainer::doList() {
	*bw_http << "<TABLE><TR class='table_body'><TD><A href='";
	*bw_http
			<< bw_http->GetServerVar("REQUEST_URL") + "&__action_name=create'>";
	*bw_http << "[+] Novo Registro</A></TD></TR></TABLE>";
	try {
		rc_table = new BWDbHtmlTable(bw_config, bw_http, *bw_db);
		rc_table->Send();
	} catch (BWError &e) {
		e.TrackError(m_BWErrorPlace, "Erro ao criad DbHtmlTable");
		throw e;
	}

	delete rc_table;
}

/*!
 \fn CGI::BWRegContainer::doMoveDown()
 */
void CGI::BWRegContainer::doMoveDown() {
	BWString query("SELECT (SELECT ");
	BWResultSet rs;
	BWString order = bw_config["orderfields"];
	BWString action;
	BWString table = rc_tablename;
	BWKeyPair set_update_up, set_update_down, where_update_up,
			where_update_down, clear_order;
	BWString lastchange = "";

	if (bw_config["keepsync"] == "true") {
		lastchange = ", ultimaalteracao = '";
		lastchange += get_date(BWMASK_DTNOW) + "'";
	}

	where_update_up["id"] = bw_http->GetParameter("__movedown_id");

	query += order + " FROM " + table + " WHERE id=" + where_update_up["id"];
	query += ") AS pr_up, id AS id_down, " + order + " AS pr_down FROM ";
	query += table + " WHERE " + order + "=(SELECT MIN(" + order + ") FROM ";
	query += table + " WHERE " + order + ">(SELECT " + order + " FROM " + table
			+ " WHERE id=";
	query += where_update_up["id"] + "))";

	rs = bw_db->ExecuteQuery(query);

	where_update_down["id"] = rs.GetColumn("id_down")[0];
	set_update_up[order] = rs.GetColumn("pr_down")[0];
	set_update_down[order] = rs.GetColumn("pr_up")[0];

	where_update_down[order] = rs.GetColumn("id")[0];

	clear_order[order] = "0";

	try {
		bw_db->BeginTransaction();
		bw_db->ExecuteUpdate(
				BWString("UPDATE ") + table + " SET " + order + "="
						+ clear_order[order] + lastchange + " WHERE id="
						+ where_update_up["id"]);
		bw_db->ExecuteUpdate(
				BWString("UPDATE ") + table + " SET " + order + "="
						+ set_update_down[order] + lastchange + " WHERE id="
						+ where_update_down["id"]);
		bw_db->ExecuteUpdate(
				BWString("UPDATE ") + table + " SET " + order + "="
						+ set_update_up[order] + lastchange + " WHERE id="
						+ where_update_up["id"]);

//     bw_db->Update(table, clear_order, where_update_up);
//     bw_db->Update(table, set_update_down, where_update_down);
//     bw_db->Update(table, set_update_up, where_update_up);
		bw_db->Commit();
	} catch (BWError &e) {
		e.TrackError(m_BWErrorPlace, "Ignorando erro ao mover");
	}

	action = bw_http->GetServerVar("REQUEST_URL");
	action.Replace("&__action_name=movedown", "");
	action.Replace(
			BWString("&__movedown_id=")
					+ bw_http->GetParameter("__movedown_id"), "");
	bw_http->SetHeader("Location", action);
}

/*!
 \fn CGI::BWRegContainer::doMoveUp()
 */
void CGI::BWRegContainer::doMoveUp() {
	BWString query("SELECT (SELECT ");
	BWResultSet rs;
	BWString order = bw_config["orderfields"];
	BWString table = rc_tablename;
	BWString action;
	BWKeyPair set_update_up, set_update_down, where_update_up,
			where_update_down, clear_order;
	BWString lastchange = "";

	if (bw_config["keepsync"] == "true") {
		lastchange = ", ultimaalteracao = '";
		lastchange += get_date(BWMASK_DTNOW) + "'";
	}

	where_update_up["id"] = bw_http->GetParameter("__moveup_id");

	query += order + " FROM " + table + " WHERE id=" + where_update_up["id"];
	query += ") AS pr_up, id AS id_down, " + order + " AS pr_down FROM ";
	query += table + " WHERE " + order + "=(SELECT MAX(" + order + ") FROM ";
	query += table + " WHERE " + order + "<(SELECT " + order + " FROM " + table
			+ " WHERE id=";
	query += where_update_up["id"] + "))";

	rs = bw_db->ExecuteQuery(query);

	where_update_down["id"] = rs.GetColumn("id_down")[0];
	set_update_up[order] = rs.GetColumn("pr_down")[0];
	set_update_down[order] = rs.GetColumn("pr_up")[0];

	where_update_down[order] = rs.GetColumn("id")[0];

	clear_order[order] = "0";
	try {
		bw_db->BeginTransaction();
		bw_db->ExecuteUpdate(
				BWString("UPDATE ") + table + " SET " + order + "="
						+ clear_order[order] + lastchange + " WHERE id="
						+ where_update_up["id"]);
		bw_db->ExecuteUpdate(
				BWString("UPDATE ") + table + " SET " + order + "="
						+ set_update_down[order] + lastchange + " WHERE id="
						+ where_update_down["id"]);
		bw_db->ExecuteUpdate(
				BWString("UPDATE ") + table + " SET " + order + "="
						+ set_update_up[order] + lastchange + " WHERE id="
						+ where_update_up["id"]);
//      bw_db->Update(table, clear_order, where_update_up);
//      bw_db->Update(table, set_update_down, where_update_down);
//      bw_db->Update(table, set_update_up, where_update_up);
		bw_db->Commit();
	} catch (BWError &e) {
		e.TrackError(m_BWErrorPlace, "Ignorando erro ao mover para cima");
	}
	action = bw_http->GetServerVar("REQUEST_URL");
	action.Replace("&__action_name=moveup", "");
	action.Replace(
			BWString("&__moveup_id=") + bw_http->GetParameter("__moveup_id"),
			"");
	bw_http->SetHeader("Location", action);
}

/*!
 \fn CGI::BWRegContainer::doDisable()
 */
void CGI::BWRegContainer::doDisable() {
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
	action.Replace(
			BWString("&__disable_id=") + bw_http->GetParameter("__disable_id"),
			"");
	bw_http->SetHeader("Location", action);
}

/*!
 \fn CGI::BWRegContainer::preForm()
 */
void CGI::BWRegContainer::preForm() {
	unsigned int i, j;
	BWList formfields;
	BWString field;

	if (bw_config["formfields"] == "(null)") {
		throw BWFatalError(CGI_ERROR, "Parametro formfields obrigatorio.");
	} else {
		formfields = bw_config["formfields"].Split(",");
	}

	for (i = 0; i < formfields.size(); i++) {
		field = formfields[i];
		cerr << "field " << field << endl;
		if (bw_config[field + ".type"] == "(null)") {
			bw_config[field + ".type"] = "text";
		}
		if (bw_config[field + ".dbfield"] == "(null)") {
			if (bw_config[field + ".relation"] == "(null)"
					|| bw_config[field + ".multirelation"] == "(null)") {
				for (j = 0; j < rc_datafields.size(); j++) {
					if (rc_datafields[j] == field) {
						bw_config[field + ".dbfield"] = rc_datafields[j];
						break;
					}
				}
			}
		}
//          cerr << "fieldlabel: " << bw_config[field + ".fieldlabel"] << endl;
		if (bw_config[field + ".label"] == "(null)") {
			bw_config[field + ".label"] = BWString(
					bw_config[field + ".fieldlabel"]);
		}
//          cerr << "label: " << bw_config[field + ".label"] << endl;
//          cerr << "fieldlabel: " << bw_config[field + ".fieldlabel"] << endl;
		bw_config[field + ".type"] = BWString(bw_config[field + ".fieldtype"]);
	}

	if (bw_config["dbtable"] == "(null)") {
		throw BWFatalError(CGI_ERROR, "Sem dbtable preenchido.");
	}

	bw_config["maintable"] = BWString(bw_config["dbtable"]);
	bw_config["fields"] = formfields.ToString();
	bw_config["name"] = "form";
}

/*!
 \fn CGI::BWRegContainer::preList()
 */
void CGI::BWRegContainer::preList() {
	BWList tabfields;
	unsigned int i;

	if (bw_config["tablefields"] == "(null)") {
		throw BWFatalError(CGI_ERROR, "tablefields nao pode ser nulo.");
	}

	tabfields = bw_config["tablefields"].Split(",");

	for (i = 0; i < tabfields.size(); i++) {
		bw_config[tabfields[i] + ".label"] = bw_config[tabfields[i]
				+ ".columnlabel"];
		bw_config[tabfields[i] + ".type"] = bw_config[tabfields[i]
				+ ".columntype"];
		bw_config[tabfields[i] + ".template"] = bw_config[tabfields[i]
				+ ".columntemplate"];
	}
}

/*!
 \fn CGI::BWRegContainer::HasDataChange(void)
 */
bool CGI::BWRegContainer::HasDataChange(void) {
	return has_data_change;
}

BWDbRelation CGI::BWRegContainer::getRelation(const BWString &field) {
	BWDbRelation rel(bw_db, bw_config["maintable"],
			bw_config[field + ".multirelation"]);

	if (bw_config["keepsync"] == "true") {
		rel.SetKeepSync();
	}

	if (bw_config["prefix"] != "(null)" && bw_config["prefix"] != "") {
		rel.SetPrefix(bw_config["prefix"]);
	}

	return rel;
}

