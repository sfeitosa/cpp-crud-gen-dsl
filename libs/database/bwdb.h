//
// C++ Interface: bwdb
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWDB_H
#define BWDB_H

#include <bwstring.h>
#include <bwresultset.h>
#include <bwurl.h>
#include <bwtemplate.h>
#include <bwkeypair.h>

#if defined(linux) || defined(__linux__)
#define DLLEXPORT
#else
#define DLLEXPORT __declspec(dllexport)
#endif

/**
Interface de classes de banco de dados

 @author Bruno M Guedes <bruno@beanweb.com.br>

 Regras de implementação de driver de banco de dados:
 * -Definir nome do driver na API
 * -Implementar classe herdando BWDb
 * -Implementar loader com a interface:
 *   BWDb *bwdb_load_nomedodriver(const BWUrl *);
 * -Implementar unloader com a interface:
 *   void bwdb_unload_nomedodriver(const BWDb *);
 * -Adicionar compilação de lib .so do banco
 * -Diretorio para libs: /srv/beanweb/lib
 * -Nome da lib: libnomedodriver.so
*/
class BWDb
{
  public:
    BWDb(const BWString &dbdrv);
    virtual ~BWDb();
    BWString GetDriverName() const;
    virtual void Open() = 0;
    virtual void Close() { return; };
    virtual void ExecuteUpdate(const BWString &cmd) = 0;
    virtual BWResultSet ExecuteQuery(const BWString &cmd) = 0;
    void setDbUsername(const BWString &u);
    void setDbPort(int p);
    virtual int GetLastId(const BWString &tablename, const BWString &idfield="id") = 0;
    bool Insert(const BWString &table, BWKeyPair &kpr);
    bool Update(const BWString &table, BWKeyPair &set, BWKeyPair &where);
    bool Update(const BWString &table, BWKeyPair &set, const BWString &where);
    bool Delete(const BWString &table, const BWString &where);
    bool Delete(const BWString &table, BWKeyPair &where);
    BWDb& BeginTransaction();
    BWDb& Commit();
    BWDb& Rollback();
    bool AtTransaction();
  protected:
    void setDriverName(const BWString &dbdrv);
    void setDbFile(const BWString &file);
    void setDbHostname(const BWString &h);
    void setDbPassword(const BWString &p);
    BWString getFinalQuery(const BWString &q);
    void setAttribute(const BWString &attribute, const BWString &value);
  protected:
    BWString db_dbname;
    BWString db_username;
    BWString db_hostname;
    int db_port;
    BWString db_file;
    BWString db_driver;
    BWString db_password;
    BWKeyPair db_attributes;
    bool at_transaction;
};

#endif
