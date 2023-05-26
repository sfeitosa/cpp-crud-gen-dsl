//
// C++ Implementation: bwajaxdbhtmlform
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <bruno@beanweb.com.br>, (C) 2013
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwajaxdbhtmlform.h"
#include <iostream>

using namespace std;

namespace CGI {

BWAjaxDbHtmlForm::BWAjaxDbHtmlForm(BWKeyPair &data, const BWString &file,
		BWDb &db) : CGI::BWAjaxHtmlForm(data, file) {
	m_data = data;
	init(db);
}

BWAjaxDbHtmlForm::BWAjaxDbHtmlForm(BWKeyPair &data, BWConfig &cfg, BWDb &db) :
		CGI::BWAjaxHtmlForm(data, cfg) {
	m_data = data;
	init(db);
}

BWAjaxDbHtmlForm::~BWAjaxDbHtmlForm() {
}
}

/*!
 \fn CGI::BWDbHtmlForm::GetFieldText(const BWString &field)
 */
BWString CGI::BWAjaxDbHtmlForm::GetFieldText(const BWString &field) {
	BWString relname;
	BWString default_value;

	default_value = m_dbreg->GetField(m_config[field + ".dbfield"]);

	if (field == m_config["name"] + "_id__") {
		m_config[field + ".default"] = m_data[field];
	} else if (m_config[field + ".multirelation"] == "(null)"
			&& m_config[field + ".relation"] == "(null)") {
		if (!(default_value == "(null)" || default_value == "")) {
			m_config[field + ".default"] = default_value;
		}
	} else {
		if (m_config[field + ".multirelation"] != "(null)"
				&& m_config[field + ".multirelation"] != "") {

			m_log(BWLog::MSG_DEBUG) << "Entrou no multirelation" << bwlog::endl;

			m_config[field + ".default"] = m_dbreg->GetRelations(field);

			m_log(BWLog::MSG_DEBUG) << "Default: "
					<< m_config[field + ".default"] << bwlog::endl;

			// Código adicionado por Samuel para tratar a especificidade dos
			// campos InputToList, que não tem tabela de multirelação. Isto
			// evita o lançamento de exceções desnecessárias ao consultar uma
			// tabela inexistente. Adicionado em 21/11/2013.
			if (m_config[field + ".fieldtype"] != "InputToList") {
				m_config[field + ".ids"] = m_dbreg->GetRelationOptionIds(field).ToString();

			    m_log(BWLog::MSG_DEBUG) << "IDs: " << m_config[field + ".ids"] << bwlog::endl;

			    m_config[field + ".descrs"] = m_dbreg->GetRelationOptionDescrs(field).ToString();

			    m_log(BWLog::MSG_DEBUG) << "Descrs: " << m_config[field + ".descrs"] << bwlog::endl;
			}

		} else {

			m_log(BWLog::MSG_DEBUG) << "Nao eh multi relation" << bwlog::endl;

			m_log(BWLog::MSG_DEBUG) << "Field: " << field << bwlog::endl;

			relname = m_config[field + ".relation"] + "_id";

			m_log(BWLog::MSG_DEBUG) << "relname: " << relname << bwlog::endl;

			m_config[field + ".default"] = m_dbreg->GetField(relname);

			m_log(BWLog::MSG_DEBUG) << "Default: "
					<< m_config[field + ".default"] << bwlog::endl;

			m_config[field + ".ids"] = m_dbreg->GetFieldOptionIds(
					m_config[field + ".relation"]).ToString();
			m_log(BWLog::MSG_DEBUG) << "IDs: " << m_config[field + ".ids"]
					<< bwlog::endl;

			m_config[field + ".descrs"] = m_dbreg->GetFieldOptionDescrs(
					m_config[field + ".relation"]).ToString();

			m_log(BWLog::MSG_DEBUG) << "Descrs: "
					<< m_config[field + ".descrs"] << bwlog::endl;
		}
	}

	return CGI::BWAjaxHtmlForm::GetFieldText(field);
}

/*!
 \fn CGI::BWDbHtmlForm::SaveForm()
 */
BWString CGI::BWAjaxDbHtmlForm::Save() {
	BWString idstr;

	if (!IsFilled()) {
		throw BWError(APP_ERROR, "Campos obrigatórios não preenchidos!");
	}

	m_dbreg->Save();
	idstr.AppendInt(m_dbreg->GetId());
	m_config[m_config["name"] + "_id__"] = idstr;

	return idstr;
}

/*!
 \fn CGI::BWDbHtmlForm::init()
 */
void CGI::BWAjaxDbHtmlForm::init(BWDb &db) {
	BWString id;

	m_db = &db;
	m_dbreg = NULL;

	try {
		m_dbreg = new BWDbRegistry(db, m_config["maintable"]);
	} catch (std::bad_alloc &ba) {
		throw BWError(CGI_ERROR,
				BWString("Ocorreu um erro ao alocar memoria: ") + ba.what());
	} catch (BWError &e) {
		e.TrackError(m_BWErrorPlace, "Erro de banco de dados");
		throw e;
	}

	m_dbreg->DisableTransactionControl();

	if (m_config["keepsync"] == "true") {
		m_dbreg->SetKeepSync();
	}

	if (m_config["prefix"] != "(null)" && m_config["prefix"] != "") {
		m_dbreg->SetTablePrefix(m_config["prefix"]);
	}

	fillRegistry();

	// Verifica o ID do cadastro, para saber se deve carregar o
	// formulário vazio ou preenchido
	id = m_data[m_config["name"] + "_id__"];

	if (id != "(null)" && id != "") {
		m_dbreg->Load(id.GetInt());
	} else {
		m_dbreg->Create();
	}

	setDataToSave();
}

void CGI::BWAjaxDbHtmlForm::fillRegistry() {
	BWList ffields = m_config["fields"].Split(",");

	m_config[m_config["name"] + "_id__.dbfield"] = "id";

	// Lendo fields
	for (unsigned int i = 0; i < ffields.size(); i++) {
		if (ffields[i] == m_config["name"] + "_id__.dbfield") {
			continue;
		}

		m_log(BWLog::MSG_XDEBUG) << "DBField: "
				<< m_config[ffields[i] + ".dbfield"] << bwlog::endl;

		// Se nao for multirelation
		if (m_config[ffields[i] + ".multirelation"] == "(null)") {
			m_log(BWLog::MSG_XDEBUG) << "Nao eh multirelation" << bwlog::endl;
			if (m_config[ffields[i] + ".dbfield"] == "(null)") {
				// Está fazendo algo nesta parte ????
				if (m_config[ffields[i] + ".relation"] != "(null)") {

				    m_log(BWLog::MSG_DEBUG) << "Adicionando SingleRelation..."
				                                << bwlog::endl;

					m_config[ffields[i] + ".dbfield"] = m_config[ffields[i]
							+ ".relation"] + "_id";

				}
				else if (m_config[ffields[i] + ".fieldtype"] == "raw") {
				    // Se o campo dbfield de um campo do tipo "RAW" não estiver
				    // preenchido, segue adiante... código utilizado para não tentar
				    // inserir informações de apresentação no banco de dados.
				    if (m_config[ffields[i] + ".dbfield"] == "(null)") {
				        continue;
				    }
				}
				else {
					BWString msg("Arquivo ");
					msg += m_config.GetPath() + ".ini incompleto: campo ";
					msg += ffields[i] + ".dbfield=";
					msg += m_config[ffields[i] + ".dbfield"];
					throw BWError(CGI_ERROR, msg);
				}
			}
			m_dbreg->AddField(m_config[ffields[i] + ".dbfield"]);
		}
		// Se for multirelation
		else {
			m_log(BWLog::MSG_DEBUG) << "Adicionando MultiRelation"
					<< bwlog::endl;

			m_dbreg->AddRelation(ffields[i], getRelation(ffields[i]));
		}
	}
}

BWDbRelation CGI::BWAjaxDbHtmlForm::getRelation(const BWString &field) {
	BWDbRelation rel(m_db, m_config["maintable"],
			m_config[field + ".multirelation"], m_config[field + ".invert"]);

	rel.UseActiveField();

	if (m_config["keepsync"] == "true") {
		rel.SetKeepSync();
	}

	if (m_config["prefix"] != "(null)" && m_config["prefix"] != "") {
		rel.SetPrefix(m_config["prefix"]);
	}

	if (m_config[field + ".prefix"] != "(null)") {
		rel.SetPrefix(m_config[field + ".prefix"]);
	}

	if (m_config[field + ".reltable"] != "(null)"
			&& m_config[field + ".reltable"] != "") {
		rel.SetRefTable(m_config[field + ".reltable"]);
	}

	if (m_config[field + ".sortfield"] != "(null)"
			&& m_config[field + ".sortfield"] != "") {
		rel.SetOrderField(m_config[field + ".sortfield"]);
	}

	return rel;
}

void CGI::BWAjaxDbHtmlForm::setDataToSave() {
	BWKeyPair kpr;
	BWKeyPair::iterator itr;
	BWString form_field;
	BWList value_list;

	if (IsFilled()) {

		kpr = GetFieldValues();
		itr = kpr.begin();

		while (itr != kpr.end()) {
			if (itr->first == m_config["name"] + "_id__") {
				itr++;
				continue;
			}

			form_field = itr->first;
			if (m_config[form_field + ".multirelation"] != "(null)"
					&& m_config[form_field + ".multirelation"] != "") {

				value_list = itr->second.Split(",");
				if (value_list.size() > 0 && value_list[0] != ""
						&& value_list[0] != "(null)") {
					/* Se nova lista de valores possui dados... */
					m_dbreg->SetRelation(form_field, value_list);
				} else {
					/* Se nova lista de valores nao possuir dados,
					 * entao o usuario queria eliminar os registros
					 */
					m_dbreg->EmptyRelation(form_field);
				}
			} else {
				m_dbreg->SetField(m_config[form_field + ".dbfield"],
						itr->second);
			}

			itr++;
		}
	}

}

