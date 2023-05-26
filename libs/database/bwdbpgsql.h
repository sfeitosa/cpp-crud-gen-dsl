#ifndef BWDBPGSQL_H
#define BWDBPGSQL_H

#include <bwdb.h>
#include <bwurl.h>
#include <libpq-fe.h>
#include <bwlist.h>

/**
Implementacao do driver do PostgreSQL para a BWAPI
Nome do driver: pgsql
Nome da biblioteca: libpgsql.so

*/
class BWDbPgSQL : public BWDb
{
  public:
    BWDbPgSQL(const BWUrl *u);
    ~BWDbPgSQL();
    void Open();
    void Close();
    void ExecuteUpdate(const BWString &cmd);
    BWResultSet ExecuteQuery(const BWString &cmd);
    int GetLastId(const BWString &tablename, const BWString &idfield="id");

  protected:
    PGconn *db_conn;
    ConnStatusType db_status;

  protected:
    PGresult *runQuery(const BWString &qry);
};

extern "C"
{
  DLLEXPORT BWDb *bwdb_load_pgsql(const BWUrl *);
  DLLEXPORT void bwdb_unload_pgsql(const BWDb *);
}

#endif
