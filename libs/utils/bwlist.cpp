#include "bwlist.h"

BWList::BWList()
    : BWPrimList()
{}


BWList::~BWList()
{}

BWList::BWList(const BWList &l) : BWPrimList(l)
{}

BWList::BWList(const BWPrimList &l) : BWPrimList(l)
{}

BWList& BWList::operator= (const BWList &l)
{
  return assign(l);
}

BWList& BWList::operator= (const BWPrimList &l)
{
  return assign(l);
}

BWList& BWList::assign(const BWPrimList &l)
{
  return static_cast<BWList&>(BWPrimList::assign(l));
}



/*!
    \fn BWList::ToString(const BWString &sep)
 */
BWString BWList::ToString(const BWString &sep, const BWString &pre, const BWString &suf)
{
  unsigned int i;
  BWString ret;

  if (!size())
  {
    return ret;
  }

  ret = pre + this[0][0] + suf;

  for (i = 1; i < this->size(); i++)
  {
    ret += sep + pre + this[0][i] + suf;
  }

  return ret;
}


/*!
    \fn BWList::ToSQLIn()
 */
BWString BWList::ToSQLIn()
{
  return BWString("(") + ToString(",", "'", "'") + ")";
}


/*!
    \fn BWList::operator[](const BWString &s)
 */
BWString& BWList::operator[](size_type i)
{
 return reinterpret_cast<BWString&>(GetString(i));
}
