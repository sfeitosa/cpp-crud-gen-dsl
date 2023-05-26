#include <web.h>
#include <iostream>

using namespace std;

int validate_session(BWDb *banco, const BWString &sessid)
{
    BWResultSet rs;
    BWString qry;

    qry  = "SELECT id FROM bw_sessao INNER JOIN bw_logon ON ";
    qry += "(bw_sessao.id = bw_logon.sessao_id) WHERE ";
    qry += "id_sessao = '" + sessid + "' AND NOW() - INTERVAL ";
    qry += "'40 seconds' <= ultimaatividade";

    rs = banco->ExecuteQuery(qry);
    if (!rs.GetNumRows())
    {
        return 0;
    }

    return rs.GetInt("id");
}

int get_session_status(BWDb *banco, const BWString &iporig) {
    BWResultSet rset;
    BWString query;

    query  = "SELECT id FROM bw_sessao INNER JOIN bw_logon ON ";
    query += "(bw_sessao.id = bw_logon.sessao_id) WHERE ";
    query += "ip = '" + iporig + "' AND NOW() - INTERVAL ";
    query += "'40 seconds' <= ultimaatividade";

    rset = banco->ExecuteQuery(query);

    if (rset.GetNumRows() > 0) {
        return rset.GetInt("id");
    }

    return 0;
}

