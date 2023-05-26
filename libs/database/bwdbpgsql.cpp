//
// C++ Implementation: bwdbpgsql
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwdbpgsql.h"
#include <iostream>

using namespace std;

BWDbPgSQL::BWDbPgSQL(const BWUrl *u)
    : BWDb("PgSQL")
{
  this->setDbPort(u->GetPort());
  this->setDbUsername(u->GetUser());
  this->setDbPassword(u->GetPass());
  this->setDbHostname(u->GetHost());
  this->db_dbname = u->GetPath().Split("/")[1];
//   cerr << "u->GetPath()=" << u->GetPath() << endl;
//   cerr << "Split(\"/\")=" << this->db_dbname << endl;
  this->Open();
  
}


BWDbPgSQL::~BWDbPgSQL()
{
  this->Close();
}

extern "C"
{
  BWDb *bwdb_load_pgsql(const BWUrl *url)
  {
    return new BWDbPgSQL(url);
  }
  void bwdb_unload_pgsql(const BWDb *db)
  {
    if (db)
    {
      delete db;
    }
    else
    {
      throw BWError(DB_ERROR, "Recurso de banco de dados invalido");
    }
  }
}



/*!
    \fn BWDbPgSQL::Open()
 */
void BWDbPgSQL::Open()
{
  BWString constring("host='");
  constring += db_hostname + "' port='";
  constring.AppendInt(db_port);
  constring += BWString("' dbname='") + db_dbname + "' user='";
  constring += db_username + "' password='" + db_password + "'";
  
//   cerr << "constring=..." << endl << constring << endl << "EOF" << endl;

  db_conn = PQconnectdb(constring.c_str());
  db_status = PQstatus(db_conn);

  if (db_status != CONNECTION_OK)
  {
    BWString erro("Erro de conex�o do banco: ");
    erro += PQerrorMessage(db_conn);
    erro += "(code=";
    erro.AppendInt(static_cast<int>(db_status));
    erro += ")";

    throw BWError(DB_ERROR, erro);
  }
}


/*!
    \fn BWDbPgSQL::Close()
 */
void BWDbPgSQL::Close()
{
  PQfinish(db_conn);
}


/*!
    \fn BWDbPgSQL::runQuery(const BWString &qry)
 */
PGresult* BWDbPgSQL::runQuery(const BWString &qry)
{
  PGresult *ret = NULL;

  ret = PQexec(db_conn, qry.c_str());

  if (PQresultStatus(ret) != PGRES_COMMAND_OK &&
      PQresultStatus(ret) != PGRES_TUPLES_OK)
  {
    BWString msg("Erro ao executar SQL: ");
    msg += PQerrorMessage(db_conn);
    msg += ": \n";
    msg += PQresultErrorMessage(ret);
    msg += " (code=";
    msg.AppendInt(static_cast<int>(PQresultStatus(ret)));
    msg += ")\nCOMANDO SQL:\n";
    msg += qry + "\n";

    throw BWError(DB_ERROR, msg);
  }

  return ret;
}

/*!
    \fn BWDbPgSQL::ExecuteUpdate(const BWString &cmd)
 */
void BWDbPgSQL::ExecuteUpdate(const BWString &cmd)
{
//   cerr << "UPDATE: " << cmd << endl;
  PQclear(this->runQuery(cmd));
}


/*!
    \fn BWDbPgSQL::ExecuteQuery(const BWString &cmd)
 */
BWResultSet BWDbPgSQL::ExecuteQuery(const BWString &cmd)
{
  PGresult *pgr = NULL;
  BWResultSet ret;
  BWList eachrow;

  int i, j, nfields;

  pgr = this->runQuery(cmd);

  nfields = PQnfields(pgr);
  for (i = 0; i < nfields; i++)
  {
    ret.AddField(PQfname(pgr, i));
  }

  for (i = 0; i < PQntuples(pgr); i++)
  {
    eachrow.clear();
    for (j = 0; j < nfields; j++)
    {
      eachrow.push_back(PQgetvalue(pgr, i, j));
    }

    ret.AppendRow(eachrow);
  }


  ret.Lock();
  PQclear(pgr);

  return ret;
}


/*!
    \fn BWDbPgSQL::GetLastId(const BWString &tablename, const BWString &idfield="id")
 */
int BWDbPgSQL::GetLastId(const BWString &tablename, const BWString &idfield)
{
  try
  {
/*
    BWResultSet rs = ExecuteQuery(BWString("SELECT currval('") +
                                  tablename + "_" + idfield + "_seq') AS ret");
*/
    BWResultSet rs = ExecuteQuery("SELECT currval(pg_get_serial_sequence('" + tablename + 
                                  "','" + idfield + "')) AS ret");

    if (!rs.GetNumRows()) {
                return -1;
    }

    return rs.GetInt("ret", 1);
  }
  catch (BWError &e)
  {
    e.TrackError(m_BWErrorPlace, "Erro ao obter ultima insercao na sessao corrente");
    throw e;
  }

  return -1;
}
