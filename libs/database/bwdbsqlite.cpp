//
// C++ Implementation: bwdbsqlite
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwdbsqlite.h"

BWDbSQLite::BWDbSQLite(const BWString &fname) : BWDb("SQLite")
{
  this->setDbFile(fname);
  this->Open();
}


BWDbSQLite::~BWDbSQLite()
{
  this->Close();
}




/*!
    \fn BWDbSQLite::Close()
 */
void BWDbSQLite::Close()
{
  sqlite3_close(m_dbrc);
}


/*!
    \fn BWDbSQLite::Open()
 */
void BWDbSQLite::Open()
{
  int ret;
  if ((ret = sqlite3_open(db_file.c_str(), &m_dbrc)) != SQLITE_OK)
  {
    throw BWError(DB_ERROR,
                  BWString("Erro ao abrir banco de dados ") +
                  db_file);
  }
}


/*!
    \fn BWDbSQLite::ExecuteUpdate(const BWString &cmd)
 */
void BWDbSQLite::ExecuteUpdate(const BWString &cmd)
{
  int cmdret;
  int retries = 2800;

//     std::cerr << cmd << std::endl;

  cmdret = sqlite3_exec(m_dbrc, cmd.c_str(), NULL, NULL, NULL);

  while (cmdret == SQLITE_BUSY && --retries)
  {
    cmdret = sqlite3_exec(m_dbrc, cmd.c_str(), NULL, NULL, NULL);
    exec_usleep(rand() % 512);
  }

  if (cmdret)
  {
    BWString erro = "Erro de SQL no banco ";
    erro += db_file + ": ";
    erro += sqlite3_errmsg(m_dbrc);
    erro += ". O erro ocorreu no comando: " + cmd;
    throw BWError(DB_ERROR, erro);
  }
}


/*!
    \fn BWDbSQLite::ExecuteQuery(const BWString &cmd)
 */
BWResultSet BWDbSQLite::ExecuteQuery(const BWString &cmd)
{
  char **resultarray = NULL;
  int rows, cols;
  int i, j, k=0;
  char *errmsg = NULL;
  BWResultSet ret;
  BWList eachrow;
  int retries = 2800;
  int retcode;
  BWString msg;
//     std::cout << "bah" << std::endl;
//     std::cerr << cmd << std::endl;

  retcode = sqlite3_get_table(m_dbrc, cmd.c_str(), &resultarray, &rows, &cols, &errmsg);
  
  while (retcode == SQLITE_BUSY && --retries)
  {
    msg = errmsg;
	sqlite3_free(errmsg);
    retcode = sqlite3_get_table(m_dbrc, cmd.c_str(), &resultarray, &rows, &cols, &errmsg);
    exec_usleep(rand() % 512);
  }

  if (retcode != SQLITE_OK)
  {
    msg = errmsg;
	sqlite3_free(errmsg);
  
    if (retcode == SQLITE_BUSY)
    {
      throw BWError(DB_ERROR,
                    BWString("O banco ") +
                    db_file +
                    " esta ocupado. Comando: " +
                    cmd);
    }
    if (resultarray)
    {
//      std::cerr << "free1" << std::endl;
      sqlite3_free_table(resultarray);
    }
    if (msg != "")
    {
      throw BWError(DB_ERROR,
                    BWString("SQLite Error: ") +
                    msg + " on query: " + cmd +
                    " dbfile=" + db_file);
    }
    else
    {
      throw BWError(DB_ERROR,
                    "Ocorreu um erro, e mensagem de erro do SQLite eh nula");
    }
  }

  for (i = 0; i < cols; i++)
  {
    if (!resultarray[k])
    {
      //resultarray[k] = (char *) "";
	  ret.AddField("");
	  k++;
    }
	else {
      ret.AddField(const_cast<const char*>(resultarray[k++]));
	}
//                cerr << "Adicionando campo " << const_cast<const char*>(resultarray[k-1]) << endl;
  }
  for (j = 1; j <= rows; j++)
  {
    eachrow.clear();
    for (i = 0; i < cols; i++)
    {
      if (!resultarray[k])
      {
        //resultarray[k] = (char *) "";
        eachrow.push_back("");
		k++;
      }
      else {
        eachrow.push_back(resultarray[k++]);
      }
//                            cerr << "Adicionando valor " << const_cast<const char*>(resultarray[k-1]) << endl;
    }
    ret.AppendRow(eachrow);
  }

//  if (resultarray && cols > 0 && rows > 0)
//  {
//    std::cerr << "free2" << std::endl;
    sqlite3_free_table(resultarray);
//  }

  ret.Lock();
  return ret;
}

extern "C"
{
  BWDb *bwdb_load_sqlite(const BWUrl *url)
  {
    BWString path = url->GetPath();
	path.erase(0, 1);
    return new BWDbSQLite(path);
  }
  void bwdb_unload_sqlite(const BWDb *db)
  {
    if (db)
    {
      delete db;
    }
    else
    {
      throw BWError(DB_ERROR,
                    "Recurso de banco de dados invalido");
    }
  }
}



/*!
    \fn BWDbSQLite::GetLastId(const BWString &tablename, const BWString &idfield)
 */
int BWDbSQLite::GetLastId(const BWString &tablename, const BWString &idfield)
{
  try
  {
    BWResultSet rs = ExecuteQuery(BWString("SELECT ") +
                                  idfield + " AS ret FROM " + tablename +
                                  " ORDER BY rowid DESC LIMIT 1");
    return rs.GetInt("ret", 1);
  }
  catch (BWError &e)
  {
    e.TrackError(m_BWErrorPlace, "Erro ao obter ultima insercao na sessao corrente");
    throw e;
  }

  return -1;
}
