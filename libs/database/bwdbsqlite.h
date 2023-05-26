//
// C++ Interface: bwdbsqlite
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWDBSQLITE_H
#define BWDBSQLITE_H

#include <sqlite3.h>
#include <stdlib.h>
#include <stdlib.h>
#include <iostream>
#include <bwdb.h>
#include <bwerror.h>
#include <bwutils.h>
/**
Driver do SQLite

 @author
*/
class BWDbSQLite : public BWDb
{
  public:
    BWDbSQLite(const BWString &fname);

    ~BWDbSQLite();
    void Close();
    void Open();
    void ExecuteUpdate(const BWString &cmd);
    BWResultSet ExecuteQuery(const BWString &cmd);
    int GetLastId(const BWString &tablename, const BWString &idfield="id");
  private:
    sqlite3 *m_dbrc;
};

extern "C"
{
  DLLEXPORT BWDb *bwdb_load_sqlite(const BWUrl *);
  DLLEXPORT void bwdb_unload_sqlite(const BWDb *);
}

#endif
