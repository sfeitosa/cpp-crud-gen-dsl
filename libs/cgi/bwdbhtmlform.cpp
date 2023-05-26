//
// C++ Implementation: bwdbhtmlform
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwdbhtmlform.h"
#include <iostream>

using namespace std;

#define DUMP(VAR) (cerr << #VAR << ": " << VAR << endl)
#define DUMPI(VAR,INDEX) (cerr << #VAR << "[" << #INDEX << "=" << INDEX << "]=" << VAR[INDEX] << endl)

namespace CGI
{

  BWDbHtmlForm::BWDbHtmlForm(const BWString &file, BWHttp *channel, BWDb &db)
      : CGI::BWHtmlForm(file,channel)
  {
    init(db);
  }

  BWDbHtmlForm::BWDbHtmlForm(BWConfig &cfg, BWHttp *channel, BWDb &db)
      : CGI::BWHtmlForm(cfg,channel)
  {
    init(db);
  }

  BWDbHtmlForm::~BWDbHtmlForm() {
  }
}


/*!
    \fn CGI::BWDbHtmlForm::GetFieldText(const BWString &field)
 */
BWString CGI::BWDbHtmlForm::GetFieldText(const BWString &field)
{
    BWString relname;
    BWString default_value;

    default_value = m_dbreg->GetField(bw_config[field + ".dbfield"]);
  
    if (field == bw_config["name"] + "_id__") {
        bw_config[field + ".default"] = bw_channel->GetParameter(field);
    }
    else if (bw_config[field + ".multirelation"] == "(null)" && 
             bw_config[field + ".relation"] == "(null)") {
        if (!(default_value == "(null)" || default_value == "")){
            bw_config[field + ".default"] = default_value;
        }
    }
    else {
        if (bw_config[field + ".multirelation"] != "(null)" && 
            bw_config[field + ".multirelation"] != "") {

            m_log(BWLog::MSG_DEBUG) << "Entrou no multirelation" << bwlog::endl;

            bw_config[field + ".default"] = m_dbreg->GetRelations(field);

            DUMP(bw_config[field + ".default"]);

            m_log(BWLog::MSG_DEBUG) << "Default: " 
                                    << bw_config[field + ".default"] 
                                    << bwlog::endl;

            bw_config[field + ".ids"] = m_dbreg->GetRelationOptionIds(
                            field).ToString();
            m_log(BWLog::MSG_DEBUG) << "IDs: " << bw_config[field + ".ids"]
                                    << bwlog::endl;

            bw_config[field + ".descrs"] = m_dbreg->GetRelationOptionDescrs(
                            field).ToString();

            m_log(BWLog::MSG_DEBUG) << "Descrs: " 
                                    << bw_config[field + ".descrs"]
                                    << bwlog::endl;
        }
        else {

            m_log(BWLog::MSG_DEBUG) << "Nao eh multi relation" << bwlog::endl;

            m_log(BWLog::MSG_DEBUG) << "Field: " << field << bwlog::endl;

            relname = bw_config[field + ".relation"] + "_id";

            m_log(BWLog::MSG_DEBUG) << "relname: " << relname << bwlog::endl;

            bw_config[field + ".default"] = m_dbreg->GetField(relname);

            m_log(BWLog::MSG_DEBUG) << "Default: " 
                                    << bw_config[field + ".default"] 
                                    << bwlog::endl;

            bw_config[field + ".ids"] = m_dbreg->GetFieldOptionIds(
                            bw_config[field + ".relation"]).ToString();
            m_log(BWLog::MSG_DEBUG) << "IDs: " << bw_config[field + ".ids"]
                                    << bwlog::endl;

            bw_config[field + ".descrs"] = m_dbreg->GetFieldOptionDescrs(
                            bw_config[field + ".relation"]).ToString();

            m_log(BWLog::MSG_DEBUG) << "Descrs: " 
                                    << bw_config[field + ".descrs"]
                                    << bwlog::endl;
 
        }
    }
  
    return CGI::BWHtmlForm::GetFieldText(field);
}


/*!
    \fn CGI::BWDbHtmlForm::SaveForm()
 */
void CGI::BWDbHtmlForm::Save()
{
  BWString idstr;

  m_dbreg->Save();
  idstr.AppendInt(m_dbreg->GetId());
  bw_config[bw_config["name"] + "_id__"] = idstr;
}



/*!
    \fn CGI::BWDbHtmlForm::init()
 */
void CGI::BWDbHtmlForm::init(BWDb &db)
{
    BWKeyRelation relations;

    bw_db = &db;
    m_dbreg = NULL;

    try {
        m_dbreg = new BWDbRegistry(db, bw_config["maintable"]);
    }
    catch(std::bad_alloc &ba) {
        throw BWError(CGI_ERROR, 
                      BWString("Ocorreu um erro ao alocar memoria: ") +
                      ba.what());
    }
    catch(BWError &e) {
        e.TrackError(m_BWErrorPlace, "Erro de banco de dados");
        throw e;
    }

    if (bw_config["keepsync"] == "true") {
        m_dbreg->SetKeepSync();
    }

    if (bw_config["prefix"] != "(null)" &&
        bw_config["prefix"] != "")
    {
        m_dbreg->SetTablePrefix(bw_config["prefix"]);
    }

    fillRegistry();

    if (bw_channel->GetParameter(bw_config["name"] + "_id__") != "(null)" && 
        bw_channel->GetParameter(bw_config["name"] + "_id__") != "") {
        m_dbreg->Load(bw_channel->GetParameter(
                       bw_config["name"] + "_id__").GetInt());
    }
    else {
        m_dbreg->Create();
    }

    setDataToSave();
}

void CGI::BWDbHtmlForm::fillRegistry() {
    BWList ffields = bw_config["fields"].Split(",");
   
    bw_config[bw_config["name"] + "_id__.dbfield"] = "id";

    // Lendo fields 
    for (unsigned int i = 0; i < ffields.size(); i++) {
        if (ffields[i] == bw_config["name"] + "_id__.dbfield") {
            continue;
        }

        m_log(BWLog::MSG_XDEBUG) << "DBField: " 
                                 << bw_config[ffields[i] + ".dbfield"]
                                 << bwlog::endl;

        // Se nao for multirelation
        if (bw_config[ffields[i] + ".multirelation"] == "(null)") {
            m_log(BWLog::MSG_XDEBUG) << "Nao eh multirelation" << bwlog::endl;
            if (bw_config[ffields[i] + ".dbfield"] == "(null)") {
                if (bw_config[ffields[i] + ".relation"] != "(null)") {
                    bw_config[ffields[i] + ".dbfield"] = bw_config[ffields[i] 
                                                        + ".relation"] + "_id";

                    m_log(BWLog::MSG_DEBUG) << "Adicionando SingleRelation..."
                                            << bwlog::endl;

                    //m_dbreg->AddRelation(ffields[i], getRelation(ffields[i]));
                }
                else {
                    BWString msg("Arquivo ");
                    msg += bw_config.GetPath() + ".ini incompleto: campo ";
                    msg += ffields[i] + ".dbfield=";
                    msg += bw_config[ffields[i] + ".dbfield"];
                    throw BWError(CGI_ERROR, msg);
                }
            }
            m_dbreg->AddField(bw_config[ffields[i] + ".dbfield"]);
        }
        // Se for multirelation
        else {
            m_log(BWLog::MSG_DEBUG) << "Adicionando MultiRelation" 
                                    << bwlog::endl;

            m_dbreg->AddRelation(ffields[i], getRelation(ffields[i]));
        }
    }
}

BWDbRelation CGI::BWDbHtmlForm::getRelation(const BWString &field) {
    BWDbRelation rel(bw_db, bw_config["maintable"], 
                     bw_config[field + ".multirelation"],
                     bw_config[field + ".invert"]);

    if (bw_config["keepsync"] == "true") {
        rel.SetKeepSync();
    }

    if (bw_config["prefix"] != "(null)" &&
        bw_config["prefix"] != "") {
        rel.SetPrefix(bw_config["prefix"]);
    }

    if (bw_config[field + ".prefix"] != "(null)") {
        rel.SetPrefix(bw_config[field + ".prefix"]);
    }

    if (bw_config[field + ".reltable"] != "(null)" &&
        bw_config[field + ".reltable"] != "") {
        rel.SetRefTable(bw_config[field + ".reltable"]);
    }

    if (bw_config[field + ".sortfield"] != "(null)" &&
        bw_config[field + ".sortfield"] != "") {
        rel.SetOrderField(bw_config[field + ".sortfield"]);
    }

    return rel;
}

void CGI::BWDbHtmlForm::setDataToSave()
{
  BWKeyPair kpr;
  BWKeyPair::iterator itr;
  BWString form_field;
  BWList value_list;

  if (IsFilled()) {

    kpr = GetFieldValues();
    itr = kpr.begin();
    while (itr != kpr.end()) {
      if (itr->first == bw_config["name"] + "_id__") {
        itr++;
        continue;
      }

      form_field = itr->first;
      if (bw_config[form_field + ".multirelation"] != "(null)" &&
          bw_config[form_field + ".multirelation"] != "") 
      {
        value_list = itr->second.Split(",");
        if (value_list.size() > 0 && value_list[0] != "" && 
            value_list[0] != "(null)") {
          /* Se nova lista de valores possui dados... */
          m_dbreg->SetRelation(form_field, value_list);
        }
        else { 
          /* Se nova lista de valores nao possuir dados, 
           * entao quereria o usuario limpa-los... 
           */
          m_dbreg->EmptyRelation(form_field);
        }
      }
      else {
        m_dbreg->SetField(bw_config[form_field + ".dbfield" ],
                           itr->second);
      }

      itr++;
    }
  }

}


