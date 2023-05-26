//
// C++ Interface: bwresultset
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWRESULTSET_H
#define BWRESULTSET_H

#include <bwkeylist.h>
#include <bwkeypair.h>
#include <bwlist.h>
#include <bwstring.h>
#include <bwerror.h>

/**
Classe para manipulacao de resultados de consultas SQL

 @author Bruno Moreira Guedes
*/
class BWResultSet : public BWKeyList
{
  public:
    BWResultSet();

    ~BWResultSet();
    BWList GetRow();
    BWList GetRow(int rn);
    BWString GetString(const BWString &cname);
    int GetInt(const BWString &cname);
    int GetPos();
    int Next();
    void Rewind();
    void SetPos(int pos);
    int GetNumRows() const;
    void AddField(const BWString &nm);
    void Lock();
    void AppendRow(BWList &row);
    BWList GetFieldList();
    BWList GetColumn(const BWString &col);
    int GetInt(const BWString &fld, int row);
    BWString GetString(const BWString &cname, int row);
    BWKeyPair GetTuple(int tuple);
    BWKeyPair GetTuple();

  protected:
    bool rs_ready;
    BWList fields;
    int cols;
    int rows;
    int rs_pos;
  private:
    void appendOnField(const BWString &field, const BWString &s);
};

#endif
