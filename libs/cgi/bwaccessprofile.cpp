//
// C++ Implementation bwaccessprofile.cpp
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2014
// 
// Created on: 15/01/2014
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwaccessprofile.h"

namespace CGI {

BWAccessProfile::BWAccessProfile(int user, const BWString &form, BWDb *db) {
    m_user = user;
    m_form = form;
    m_db = db;
}

BWAccessProfile::~BWAccessProfile() {
}

} /* namespace cgi */

bool CGI::BWAccessProfile::CanAccess() {
    BWString query;
    BWResultSet rset;

    if (m_user == 0) {
        return true;
    }

    query  = "SELECT bw_usuario.id FROM bw_perfil_formulario ";
    query += "INNER JOIN bw_formulario ON ";
    query += "bw_perfil_formulario.formulario_id = bw_formulario.id ";
    query += "INNER JOIN bw_perfil ON ";
    query += "bw_perfil_formulario.perfil_id = bw_perfil.id ";
    query += "INNER JOIN bw_usuario ON ";
    query += "bw_perfil.id = bw_usuario.perfil_id ";
    query += "WHERE bw_usuario.id = ";
    query.AppendInt(m_user);
    query += " AND bw_formulario.nome = '" + m_form + "'";

    rset = m_db->ExecuteQuery(query);

    if (rset.GetNumRows() > 0) {
        return true;
    }

    return false;
}


bool CGI::BWAccessProfile::CanCreate() {
    BWString query;
    BWResultSet rset;

    if (m_user == 0) {
        return true;
    }

    query  = "SELECT criar FROM bw_perfil_formulario ";
    query += "INNER JOIN bw_formulario ON ";
    query += "bw_perfil_formulario.formulario_id = bw_formulario.id ";
    query += "INNER JOIN bw_perfil ON ";
    query += "bw_perfil_formulario.perfil_id = bw_perfil.id ";
    query += "INNER JOIN bw_usuario ON ";
    query += "bw_perfil.id = bw_usuario.perfil_id ";
    query += "WHERE bw_usuario.id = ";
    query.AppendInt(m_user);
    query += " AND bw_formulario.nome = '" + m_form + "'";

    rset = m_db->ExecuteQuery(query);

    if (rset.GetNumRows() > 0) {
        if (rset.GetString("criar") == "S") {
            return true;
        }
    }

    return false;
}

BWString CGI::BWAccessProfile::CanList() {
    BWString query;
    BWResultSet rset;

    if (m_user == 0) {
        return "T";
    }

    query  = "SELECT visualizar FROM bw_perfil_formulario ";
    query += "INNER JOIN bw_formulario ON ";
    query += "bw_perfil_formulario.formulario_id = bw_formulario.id ";
    query += "INNER JOIN bw_perfil ON ";
    query += "bw_perfil_formulario.perfil_id = bw_perfil.id ";
    query += "INNER JOIN bw_usuario ON ";
    query += "bw_perfil.id = bw_usuario.perfil_id ";
    query += "WHERE bw_usuario.id = ";
    query.AppendInt(m_user);
    query += " AND bw_formulario.nome = '" + m_form + "'";

    rset = m_db->ExecuteQuery(query);

    if (rset.GetNumRows() > 0) {
        return rset.GetString("visualizar");
    }

    return "T";
}


BWString CGI::BWAccessProfile::CanEdit() {
    BWString query;
    BWResultSet rset;

    if (m_user == 0) {
        return "T";
    }

    query  = "SELECT editar FROM bw_perfil_formulario ";
    query += "INNER JOIN bw_formulario ON ";
    query += "bw_perfil_formulario.formulario_id = bw_formulario.id ";
    query += "INNER JOIN bw_perfil ON ";
    query += "bw_perfil_formulario.perfil_id = bw_perfil.id ";
    query += "INNER JOIN bw_usuario ON ";
    query += "bw_perfil.id = bw_usuario.perfil_id ";
    query += "WHERE bw_usuario.id = ";
    query.AppendInt(m_user);
    query += " AND bw_formulario.nome = '" + m_form + "'";

    rset = m_db->ExecuteQuery(query);

    if (rset.GetNumRows() > 0) {
        return rset.GetString("editar");
    }

    return "T";
}

BWString CGI::BWAccessProfile::CanDelete() {
    BWString query;
    BWResultSet rset;

    if (m_user == 0) {
        return "T";
    }

    query  = "SELECT excluir FROM bw_perfil_formulario ";
    query += "INNER JOIN bw_formulario ON ";
    query += "bw_perfil_formulario.formulario_id = bw_formulario.id ";
    query += "INNER JOIN bw_perfil ON ";
    query += "bw_perfil_formulario.perfil_id = bw_perfil.id ";
    query += "INNER JOIN bw_usuario ON ";
    query += "bw_perfil.id = bw_usuario.perfil_id ";
    query += "WHERE bw_usuario.id = ";
    query.AppendInt(m_user);
    query += " AND bw_formulario.nome = '" + m_form + "'";

    rset = m_db->ExecuteQuery(query);

    if (rset.GetNumRows() > 0) {
        return rset.GetString("excluir");
    }

    return "T";
}

