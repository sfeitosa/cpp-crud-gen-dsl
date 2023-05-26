//
// C++ Implementation: bwdb
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwdb.h"

BWDb::BWDb(const BWString &dbdrv)
{
  setDriverName(dbdrv);
  at_transaction = false;
}


BWDb::~BWDb()
{
  Close();
}

void BWDb::setDriverName(const BWString &dbdrv)
{
  db_driver = dbdrv;
}


/*!
    \fn BWDb::setDbFile(const BWString &file)
 */
void BWDb::setDbFile(const BWString &file)
{
  db_file = file;
}


/*!
    \fn BWDb::setDbHostname(const BWString &h)
 */
void BWDb::setDbHostname(const BWString &h)
{
  db_hostname = h;
}


/*!
    \fn BWDb::setDbPassword(const BWString &p)
 */
void BWDb::setDbPassword(const BWString &p)
{
  db_password = p;
}


/*!
    \fn BWDb::setDbUsername(const BWString &u)
 */
void BWDb::setDbUsername(const BWString &u)
{
  db_username = u;
}


/*!
    \fn BWDb::setDbPort(int p)
 */
void BWDb::setDbPort(int p)
{
  db_port = p;
}


/*!
    \fn BWDb::getFinalQuery(const BWString &q)
 */
BWString BWDb::getFinalQuery(const BWString &q)
{
  BWTemplate tpl(q, db_attributes);
  return tpl.GetText();
}


/*!
    \fn BWDb::setAttribute(const BWString &attribute, const BWString &value)
 */
void BWDb::setAttribute(const BWString &attribute, const BWString &value)
{
  db_attributes[attribute] = value;
}


/*!
    \fn BWDb::Insert(const BWString &table, BWKeyPair &kpr)
 */
bool BWDb::Insert(const BWString &table, BWKeyPair &kpr)
{
  BWString inscmd("INSERT INTO "), values(" VALUES(");
  BWKeyPair::iterator itr;

  inscmd += table + "(";
  if (!kpr.size())
  {
    throw BWError(DB_ERROR,
                  BWString("Fieldset vazio ao inserir em ") + table);
    return false;
  }
  itr = kpr.begin();
  if (itr->second != "" && itr->second != "(null)") {
    inscmd += itr->first;
    inscmd += ", ";
    if (itr->second == "<%#BWDB_NULL_VALUE#%>") {
        values += "NULL";
    }
    else {
        values += "'" + itr->second.Escape() + "'";
    }
    values += ", ";
  }

  while (++itr != kpr.end())
  {
    if (itr->second != "" && itr->second != "(null)") {
      inscmd += itr->first;
      inscmd += ", ";
      if (itr->second == "<%#BWDB_NULL_VALUE#%>") {
          values += "NULL";
      }
      else {
          values += "'" + itr->second.Escape() + "'";
      }
      values += ", ";
    }
  }

  inscmd.erase(inscmd.size() - 2);
  values.erase(values.size() - 2);

  inscmd += ")";
  inscmd += values + ")";

  try
  {
    ExecuteUpdate(inscmd);
  }
  catch (BWError &e)
  {
    e.TrackError(m_BWErrorPlace, "Erro de ::Insert()");
    if (e.IsFatal())
    {
      throw e;
    }
    else
    {
      return false;
    }
  }
  return true;
}


/*!
    \fn BWDb::Update(const BWString &table, BWKeyPair &set, const BWString &where)
 */
bool BWDb::Update(const BWString &table, BWKeyPair &set, const BWString &where)
{
  BWString updcmd("UPDATE ");
  updcmd += table + " SET " + set.ToString("=", ", ") +
            " WHERE " + where;

  std::cerr << "BWDb::Update::SQL: " << updcmd << std::endl;
  try
  {
    ExecuteUpdate(updcmd);
  }
  catch (BWError &e)
  {
    e.TrackError(m_BWErrorPlace, "Erro de ::Update()");
    if (e.IsFatal())
    {
      throw e;
    }
    else
    {
      return false;
    }
  }
  return true;
}

/*!
    \fn BWDb::Update(const BWString &table, BWKeyPair &set, BWKeyPair &where)
 */
bool BWDb::Update(const BWString &table, BWKeyPair &set, BWKeyPair &where)
{
  return Update(table, set, where.ToSQLWhere());
}


/*!
    \fn BWDb::Delete(const BWString &table, const BWString &where)
 */
bool BWDb::Delete(const BWString &table, const BWString &where)
{
  BWString delecmd("DELETE FROM ");

  if (where == "")
  {
    throw BWError(DB_ERROR,
                  "DELETE sem clausula where e perigoso. Execute manualmente o comando");
  }

  delecmd += table + " WHERE " + where;

  try
  {
    ExecuteUpdate(delecmd);
  }
  catch (BWError &e)
  {
    e.TrackError(m_BWErrorPlace, "Erro  de ::Delete()");
    if (e.IsFatal())
    {
      throw e;
    }
    else
    {
      return false;
    }
  }
  return true;
}

/*!
    \fn BWDb::Delete(const BWString &table, BWKeyPair &where)
 */
bool BWDb::Delete(const BWString &table, BWKeyPair &where)
{
  return Delete(table, where.ToSQLWhere());
}



/*!
    \fn BWDb::BeginTransaction()
 */
BWDb& BWDb::BeginTransaction()
{
  if (AtTransaction())
  {
    throw BWError(DB_ERROR, "Banco ja esta em transacao");
  }

  try
  {
    ExecuteUpdate("BEGIN TRANSACTION");
    at_transaction = true;
  }
  catch (BWError &e)
  {
    e.TrackError(m_BWErrorPlace, "Erro ao iniciar transacao");
    throw e;
  }

  return *this;
}


/*!
    \fn BWDb::Commit()
 */
BWDb& BWDb::Commit()
{
  if (!AtTransaction())
  {
    throw BWError(DB_ERROR, "Banco nao esta em transacao");
  }

  try
  {
    ExecuteUpdate("COMMIT");
    at_transaction = false;
  }
  catch (BWError &e)
  {
    e.TrackError(m_BWErrorPlace, "Erro ao salvar dados de transacao");
    throw e;
  }

  return *this;
}


/*!
    \fn BWDb::Rollback()
 */
BWDb& BWDb::Rollback()
{
  if (!AtTransaction())
  {
    throw BWError(DB_ERROR, "Banco nao esta em transacao");
  }

  try
  {
    ExecuteUpdate("ROLLBACK");
    at_transaction = false;
  }
  catch (BWError &e)
  {
    e.TrackError(m_BWErrorPlace, "Erro ao cancelar transacao");
  }

  return *this;
}


/*!
    \fn BWDb::AtTransaction()
 */
bool BWDb::AtTransaction()
{
  return at_transaction;
}

