//
// C++ Implementation: bwresultset
//
// Description:
//
//
// Author: Brun Moreira Guedes <bruno@beanweb.com.br>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwresultset.h"

BWResultSet::BWResultSet()
    : BWKeyList()
{
  rs_ready = false;
  SetPos(-1);
  rows = 0;
}


BWResultSet::~BWResultSet()
{}




/*!
    \fn BWResultSet::GetRow()
 */
BWList BWResultSet::GetRow()
{
  if (rs_pos < 0)
  {
    rs_pos++;
  }
  return GetRow(rs_pos);
}


/*!
    \fn BWResultSet::GetRow(int rn)
 */
BWList BWResultSet::GetRow(int rn)
{
  unsigned int i;
  BWList ret;

  if (!rs_ready)
  {
    throw BWError(DB_ERROR, "Resultset ainda nao esta pronto");
  }

  for (i = 0; i < fields.size(); i++)
  {
    ret.push_back(this[0][ fields[i] ][rn-1]);
  }

  return ret;
}


/*!
    \fn BWResultSet::GetString(const BWString &cname)
 */
BWString BWResultSet::GetString(const BWString &cname)
{
//             std::cout << ":" << rs_pos << std::endl;
  if (rs_pos < 0)
  {
    rs_pos = 0;
  }
  
  return GetString(cname, rs_pos+1);

}


/*!
    \fn BWResultSet::GetInt(const BWString &cname)
 */
int BWResultSet::GetInt(const BWString &cname)
{
  return GetString(cname).GetInt();
}


/*!
    \fn BWResultSet::GetPos()
 */
int BWResultSet::GetPos()
{
  return rs_pos;
}


/*!
    \fn BWResultSet::Next()
 */
int BWResultSet::Next()
{
  if (!rows)
  {
    return 0;
  }
  if (rs_pos + 1 >= rows)
  {
    rs_pos = 0;
    return 0;
  }
//     std::cout << "rs_pos: " << rs_pos << std::endl;
  rs_pos++;
//     std::cout << "rs_pos: " << rs_pos << std::endl;
  return 1;
}


/*!
    \fn BWResultSet::Rewind()
 */
void BWResultSet::Rewind()
{
  SetPos(-1);
}


/*!
    \fn BWResultSet::SetPos(int pos)
 */
void BWResultSet::SetPos(int pos)
{
  if (pos >= -1)
  {
    rs_pos = pos;
  }
}


/*!
    \fn BWResultSet::GetNumRows()
 */
int BWResultSet::GetNumRows() const
{
  return rows;
}


/*!
    \fn BWResultSet::AddField(const BWString &nm)
 */
void BWResultSet::AddField(const BWString &nm)
{
  if (rs_ready)
  {
    throw BWError(DB_ERROR,  "ResultSet ja esta pronto");
  }
  fields.push_back(nm);
}


/*!
    \fn BWResultSet::Lock()
 */
void BWResultSet::Lock()
{
  rs_ready = true;
  Rewind();
}


/*!
    \fn BWResultSet::appendOnField(const BWString &field, const BWString &s)
 */
void BWResultSet::appendOnField(const BWString &field, const BWString &s)
{
  //__BASIC_KEYLIST::operator[](field).push_back(s);
  this[0][field].push_back(s);
}


/*!
    \fn BWResultSet::AppendRow(const BWList &row)
 */
void BWResultSet::AppendRow(BWList &row)
{
  const unsigned int siz = row.size();
  unsigned int i;
  BWString field, value;

  if (siz != fields.size())
  {
    throw BWError(DB_ERROR,  "ROW Invalido");
  }

  for (i = 0; i < siz; i++)
  {
    field = fields[i];
    value = row[i];
//                 std::cerr << "Appendando " << value << " no campo " << field << std::endl;
    appendOnField(field, value);
    //this[0][field].push_back(value);
  }

  rows++;
}


/*!
    \fn BWResultSet::GetFieldList()
 */
BWList BWResultSet::GetFieldList()
{
  return fields;
}


/*!
    \fn BWResultSet::GetColumn(const BWString &col)
 */
BWList BWResultSet::GetColumn(const BWString &col)
{
  return this[0][col];
}


/*!
    \fn BWResultSet::GetInt(const BWString &fld, int row)
 */
int BWResultSet::GetInt(const BWString &fld, int row)
{
    return GetString(fld, row).GetInt();
}


/*!
    \fn BWResultSet::GetString(const BWString &cname, int row)
 */
BWString BWResultSet::GetString(const BWString &cname, int row)
{
            if (!rs_ready)
            {
                        throw BWError(DB_ERROR,  "Resultset ainda nao esta pronto");
            }

            if (GetNumRows() <= rs_pos)
            {
                        throw BWError(DB_ERROR,  "Limite do resultset ja encontrado");
            }
            return this[0][cname][row-1];
}


/*!
    \fn BWResultSet::GetTuple(int tuple)
 */
BWKeyPair BWResultSet::GetTuple(int tuple)
{
    BWKeyPair ret;
    BWList flds = GetFieldList();
    unsigned int i;
    for (i = 0; i < flds.size(); i++) {
                if (flds[i] != "(null)") {
                        ret[flds[i]] = GetString(flds[i]);
                }
    }
    
    return ret;
}


/*!
    \fn BWResultSet::GetTuple()
 */
BWKeyPair BWResultSet::GetTuple()
{
 if (rs_pos < 0)
 {
  rs_pos = 0;
 }
  
 return GetTuple(rs_pos + 1);
}
