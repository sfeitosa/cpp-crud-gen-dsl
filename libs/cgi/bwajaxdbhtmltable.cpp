//
// C++ Implementation: bwajaxdbhtmltable
//
// Description: 
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2013
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwajaxdbhtmltable.h"

namespace CGI {

BWAjaxDbHtmlTable::BWAjaxDbHtmlTable(BWKeyPair &data, const BWString &cfg, BWDb &db)
 : BWAjaxHtmlTable(data, cfg)
{
     m_db = &db;
     m_nrpages = "";
}

BWAjaxDbHtmlTable::BWAjaxDbHtmlTable(BWKeyPair &data, BWConfig &cfg, BWDb &db)
            : BWAjaxHtmlTable(data, cfg)
{
     m_db = &db;
     m_nrpages = "";
}

BWAjaxDbHtmlTable::~BWAjaxDbHtmlTable()
{
}

}

/*!
    \fn CGI::BWDbHtmlTable::GetInnerHTML()
 */
BWString CGI::BWAjaxDbHtmlTable::GetInnerHTML()
{
     BWString innerHTML;
     prepareHtmlTable();
     innerHTML = BWAjaxHtmlTable::htmlTableGetInnerHTML();
     return innerHTML;
}


/*!
    \fn CGI::BWDbHtmlTable::prepareHtmlTable()
 */
void CGI::BWAjaxDbHtmlTable::prepareHtmlTable()
{
     BWString query;
     BWResultSet dbreturn;
     BWList fields, options;
     unsigned int i;
     BWAccessProfile prof(m_data["BWC_USER_ID"].GetInt(), m_data["cad"], m_db);
     BWString newoptions = "";

     if (prof.CanList() == "P") {
         // Somente trazer os dados criados pelo
         // usuario logado
         query = generateQuery(true);
     }
     else {
         // Trazer todos os dados
         query = generateQuery(false);
     }

     if (m_config["table.offset"] != "(null)" && m_config["table.offset"] != "") {
         m_nrpages = getTotalPages();
     }

     // Tratamento de permissão de acesso na criação dos botões de manipulação de dados
     if (m_config["tableoptions"] != "(null)" && m_config["tableoptions"] != "") {
         options = m_config["tableoptions"].Split(",");
         for (i = 0; i < options.size(); i++) {
             if (options[i] == "edit" && prof.CanEdit() != "N") {
                 newoptions += "edit";
             }
             if (options[i] == "delete" && prof.CanDelete() != "N") {
                 if (newoptions != "") {
                     newoptions += ",";
                 }
                 newoptions += "delete";
             }
             m_config["tableoptions"] = newoptions;
         }
     }

     try {
            dbreturn = m_db->ExecuteQuery(query);
     }
     catch (BWError &e) {
            e.TrackError(m_BWErrorPlace,
                         "Erro carregando dados da tabela");
            throw e;
     }
     
     fields = m_config["datafields"].Split(",");

     for (i = 0; i < fields.size(); i++) {
          m_config[fields[i] + ".values"] = dbreturn.GetColumn(fields[i]).ToString();
     }
}

/*!
    \fn CGI::BWDbHtmlTable::generateQuery()
 */
BWString CGI::BWAjaxDbHtmlTable::generateQuery(bool justowner)
{
     BWString query;
     BWString selfields = "";
     BWString relfields = "";
     BWString reljoins = "";
     BWString reftable;
     BWString prefix;
     BWString tablename;
     BWList fields;
     unsigned int i;
     int offset;

     if (m_config["prefix"] != "(null)" && m_config["prefix"] != "") {
         prefix = m_config["prefix"] + "_";
     }

     tablename = prefix + m_config["dbtable"];

     fields = m_config["datafields"].Split(",");

     for (i = 0; i < fields.size(); i++) {
    	 selfields += tablename + "." + fields[i] + ",";
     }

     selfields.erase(selfields.length() - 1);

     fields = m_config["tablefields"].Split(",");

     for (i = 0; i < fields.size(); i++) {
    	 if (m_config[fields[i] + ".relation"] != "(null)") {
    		 if (m_config[fields[i] + ".reftable"] != "(null)") {
    			 reftable = m_config[fields[i] + ".reftable"];
    		 }
    		 else {
                 reftable = prefix + m_config[fields[i] + ".relation"];
    		 }

    		 if (m_config[fields[i] + ".descrfield"] != "(null)") {
    			 relfields += reftable + "." + m_config[fields[i] + ".descrfield"];
    			 relfields += " AS " + fields[i] + ",";
    		 }
    		 else {
    			 relfields += reftable + "." + "descr AS " + fields[i] + ",";
    		 }

    		 if (m_config[fields[i] + ".idfield"] != "(null)") {
                 reljoins += " INNER JOIN " + reftable + " ON ";
                 reljoins += tablename + "." +  m_config[fields[i] + ".relation"] + "_id = ";
                 reljoins += reftable + "." + m_config[fields[i] + ".idfield"];
             }
             else {
                 reljoins += " INNER JOIN " + reftable + " ON ";
                 reljoins += tablename + "." +  m_config[fields[i] + ".relation"] + "_id = ";
                 reljoins += reftable + ".id";
             }

    		 m_config["datafields"] += "," + fields[i];
    	 }
     }

     query  = "SELECT " + relfields + selfields + " FROM ";
     query += tablename + reljoins;
     query += " WHERE " + tablename + ".ativo = 'S'";

     if (justowner) {
         query += " AND " + tablename + ".creator = " + m_data["BWC_USER_ID"];
     }

     if (m_config["orderfields"] != "(null)" && m_config["orderfields"] != "") {
         query += " ORDER BY " + m_config["orderfields"];
         if (m_config["orderseq"] == "ASC" || m_config["orderseq"] == "DESC") {
             query += " " + m_config["orderseq"];
         }
     }

     if (m_config["table.offset"] != "(null)" && m_config["table.offset"] != "") {
    	 if (m_data["curpage"] == "(null)") {
    		 m_data["curpage"] = "1";
    	 }
         offset = (m_data["curpage"].GetInt() - 1) * m_config["table.offset"].GetInt();
         query += " LIMIT " + m_config["table.offset"] + " OFFSET ";
         query.AppendInt(offset);
     }

     m_log(BWLog::MSG_XDEBUG) << "BWAjaxDbHtmlTable::generateQuery -> query: " << query << bwlog::endl;

     return query;
}

/*!
    \fn CGI::BWDbHtmlTable::getTotalPages()
 */
BWString CGI::BWAjaxDbHtmlTable::getTotalPages()
{
     BWString query;
     BWString prefix;
     BWString tablename;
     BWResultSet rset;
     int total;

     if (m_config["prefix"] != "(null)" && m_config["prefix"] != "") {
         prefix = m_config["prefix"] + "_";
     }

     tablename = prefix + m_config["dbtable"];

     query  = "SELECT COUNT(id) AS nrreg FROM ";
     query += tablename;
     query += " WHERE " + tablename + ".ativo = 'S'";

     try {
            rset = m_db->ExecuteQuery(query);
     }
     catch (BWError &e) {
            e.TrackError(m_BWErrorPlace,
                         "Erro carregando dados da tabela");
            throw e;
     }

     total = ceil((float) rset.GetInt("nrreg") / m_config["table.offset"].GetInt());

     return BWString("").AppendInt(total);
}
