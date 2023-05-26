#include "bwdbhtmltable.h"

namespace CGI {

BWDbHtmlTable::BWDbHtmlTable(const BWString& cfg, BWHttp *channel, BWDb &db)
 : BWHtmlTable(cfg, channel)
{
     bw_db = &db;
}

BWDbHtmlTable::BWDbHtmlTable(BWConfig& cfg, BWHttp *channel, BWDb &db)
            : BWHtmlTable(cfg, channel)
{
     bw_db = &db;
}

BWDbHtmlTable::~BWDbHtmlTable()
{
}


/*!
    \fn CGI::BWDbHtmlTable::GetInnerHTML()
 */
BWString CGI::BWDbHtmlTable::GetInnerHTML()
{
     BWString innerHTML;
     prepareHtmlTable();
     innerHTML = BWHtmlTable::htmlTableGetInnerHTML();
     return innerHTML;
}


/*!
    \fn CGI::BWDbHtmlTable::prepareHtmlTable()
 */
void CGI::BWDbHtmlTable::prepareHtmlTable()
{
     BWString tablename, query;
     BWResultSet dbreturn;
     BWList fields;
     unsigned int i;

     if (bw_config["prefix"] != "(null)" && bw_config["prefix"] != "") {
         tablename = bw_config["prefix"] + "_";
     }
     tablename += bw_config["dbtable"];
     
     fields = bw_config["datafields"].Split(",");
     query = "SELECT " + bw_config["datafields"] + " FROM " + tablename;
     if (bw_config["orderfields"] != "(null)" && bw_config["orderfields"] != "") {
         query += " ORDER BY " + bw_config["orderfields"];
         if (bw_config["orderseq"] == "ASC" || bw_config["orderseq"] == "DESC") {
             query += " " + bw_config["orderseq"];
         }
     }
     
     try {
            dbreturn = bw_db->ExecuteQuery(query);
     }
     catch (BWError &e) {
            e.TrackError(m_BWErrorPlace,
                         "Erro carregando dados da tabela");
            throw e;
     }
     
     for (i = 0; i < fields.size(); i++) {
          bw_config[fields[i] + ".values"] = dbreturn.GetColumn(fields[i]).ToString();
     }
}

}
