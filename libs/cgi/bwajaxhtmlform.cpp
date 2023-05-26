//
// C++ Implementation: bwajaxhtmlform
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2013
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwajaxhtmlform.h"

namespace CGI {

using namespace Http;

BWAjaxHtmlForm::BWAjaxHtmlForm(BWKeyPair &data, const BWString &file) {
	BWConfig cfg;

	m_data = data;
	cfg.Load(file);

	init(cfg);
}

BWAjaxHtmlForm::BWAjaxHtmlForm(BWKeyPair &data, BWConfig &cfg) {
	m_data = data;
	init(cfg);
}

BWAjaxHtmlForm::~BWAjaxHtmlForm() {
}
}

/*!
 @fn CGI::BWHtmlForm::GetFieldValues()
 */
BWKeyPair CGI::BWAjaxHtmlForm::GetFieldValues() {
	unsigned int i;
	BWKeyPair ret;
	BWString cur;

	for (i = 0; i < m_fields.size(); i++) {
		cur = m_fields[i];
		ret[cur] = this[0][cur];
	}

	return ret;
}

/*!
 \fn CGI::BWHtmlForm::operator[](const BWString &idx)
 */
BWString CGI::BWAjaxHtmlForm::operator[](const BWString &idx) {
	BWString mfields = m_config[idx + ".multifields"];
	BWList subfields;
	unsigned int i;
	BWString ret;

	if (mfields != "(null)") {
		subfields = mfields.Split(",");

		ret = subfields[0];
		for (i = 1; i < subfields.size(); i++) {
			ret += ",";
			ret += m_data[subfields[i]];
		}
	} else {
		ret = m_data[idx];
	}
	return ret;
}

/*!
 \fn CGI::BWHtmlForm::IsFilled()
 */
bool CGI::BWAjaxHtmlForm::IsFilled() {
	unsigned int i;
	BWList mandatory = m_config["mandatory"].Split(",");

	if (m_config["mandatory"] != "(null)") {
	    for (i = 0; i < mandatory.size(); i++) {
		    if (this[0][mandatory[i]] == "(null)" ||
		        this[0][mandatory[i]] == "") {
			    return false;
		    }
	    }
	}

	return true;
}

/*!
 \fn CGI::BWHtmlForm::WasSubmitted()
 */
bool CGI::BWAjaxHtmlForm::WasSubmitted() {
	if (m_data["submitted"] == "true") {
		return true;
	}

	return false;
}

/*!
 \fn CGI::BWHtmlForm::AddField(const BWString &name, const BWString &type, const BWString &label, const BWString &default)
 */
void CGI::BWAjaxHtmlForm::AddField(const BWString &name, const BWString &type,
		const BWString &label, const BWString &defaultv) {
	if (name == "(null)") {
		throw BWError(HTTP_ERROR,
				"Nao foi possivel adicionar campo com nome vazio");
	}
	m_config["fields"] += ",";
	m_config["fields"] += name;
	m_fields.push_back(name);

	m_config[name + ".type"] = type;
	m_config[name + ".label"] = label;
	m_config[name + ".default"] = defaultv;
}

/*!
 \fn CGI::BWHtmlForm::GetInnerHtml()
 */
BWString CGI::BWAjaxHtmlForm::GetInnerHtml() {
	unsigned int i;
	BWTemplate modal;
	BWTemplate form;
	BWString html = "";
	BWString action = "?cad=";
	BWString id;

	m_log(BWLog::MSG_XDEBUG) << "BWAjaxHtmlForm::GetInnerHtml" << bwlog::endl;

	modal.FromFile(_BW_HTML_MODAL);
	form.FromFile(_BW_HTML_FORM);

	modal.SetAttribute("TITLE", m_config["label"]);

	for (i = 0; i < m_fields.size(); i++) {
		html += GetFieldText(m_fields[i]);
	}

	action += m_data["cad"];
	action += "&__action_name=";
	action += m_data["__action_name"];

	id = m_data[m_config["name"] + "_id__"];

	if (id != "(null)") {
		action += "&" + m_config["name"] + "_id__=";
		action += id;
	}

	action += "&submitted=true";

	form.SetAttribute("CONTENT", html);
	form.SetAttribute("FORM_NAME", m_data["cad"]);
	form.SetAttribute("FORM_ACTION", action);
	modal.SetAttribute("CONTENT", form.GetText());

	if (m_config["button_ok"] == "(null)") {
		modal.SetAttribute("BUTTON_OK", "Salvar Alterações");
	}
	else {
	    modal.SetAttribute("BUTTON_OK", m_config["button_ok"]);
	}

	return modal.GetText();
}

/*!
 \fn CGI::BWHtmlForm::SetConfig(const BWString &parm, const BWString &val)
 */
/*
void CGI::BWHtmlForm::SetConfig(const BWString &parm, const BWString &val) {
	bw_config[parm] = val;
}
*/

/*!
 \fn CGI::BWHtmlForm::SetFieldParm(BWString &field, const BWString &parm, const BWString &value)
 */
/*void CGI::BWHtmlForm::SetFieldParm(BWString &field, const BWString &parm,
		const BWString &value) {
	m_config[field + "." + parm] = value;
}*/

/*!
 \fn CGI::BWHtmlForm::htmlFormGetFieldText(const BWString &field)
 */

BWString CGI::BWAjaxHtmlForm::htmlFormGetFieldText(const BWString &field) {
	BWString templf(_BW_HTML_FIELDTYPES);
	BWTemplate tpl;
	BWList params;
	BWList descrs;

	templf += m_config[field + ".type"] + ".fft";

	tpl.FromFile(templf);

	tpl.SetAttribute("FIELD_TYPE", m_config[field + ".type"]);
	tpl.SetAttribute("FIELD_LABEL", m_config[field + ".label"]);
	tpl.SetAttribute("FIELD_SUBLABEL", m_config[field + ".sublabel"]);
	tpl.SetAttribute("FIELD_HELPER", m_config[field + ".helptext"]);
	tpl.SetAttribute("FIELD_NAME", field);
	tpl.SetAttribute("FIELD_ICON", m_config[field + ".icon"]);
	tpl.SetAttribute("FIELD_RAWHTML", m_config[field + ".html"]);

	if (this[0][field] == "(null)") {
		tpl.SetAttribute("FIELD_VALUE", m_config[field + ".default"]);
	}
	else {
		tpl.SetAttribute("FIELD_VALUE", this[0][field]);
	}

	if (m_config[field + ".ids"] != "(null)" &&
		m_config[field + ".descrs"] != "(null)") {

		params = m_config[field + ".ids"].Split(",");
		descrs = m_config[field + ".descrs"].Split(",");

		tpl.SetAttributeList("FIELDOPT_ID", params);
		tpl.SetAttributeList("FIELDOPT_DESCR", descrs);
	}

	if (m_config[field + ".multilabels"] != "(null)") {
		params = m_config[field + ".multilabels"].Split(",");
		tpl.SetAttributeList("FIELD_SUBLABEL", params);
	}
	if (m_config[field + ".multifields"] != "(null)") {
		params = m_config[field + ".multifields"].Split(",");
		tpl.SetAttributeList("FIELD_SUBFIELD", params);
	}
	if (m_config[field + ".inputsep"] != "(null)") {
		tpl.SetAttribute("FIELD_INPUTSEP", m_config[field + ".inputsep"]);
	}

	return tpl.GetText();
}

/*!
 \fn CGI::BWHtmlForm::init(BWConfig &cfg, BWHttp *channel)
 */
void CGI::BWAjaxHtmlForm::init(BWConfig &cfg) {
	BWList mandatory;
	BWString curlabel;
	unsigned int i;

	m_config = cfg;

	AddField(m_config["name"] + "_id__", "hidden", "", "");

	mandatory = m_config["mandatory"].Split(",");
	for (i = 0; i < mandatory.size(); i++) {
		curlabel = m_config[mandatory[i] + ".label"];
		curlabel += "*";
		m_config[mandatory[i] + ".label"] = curlabel;
	}

	m_fields = m_config["fields"].Split(",");
}
