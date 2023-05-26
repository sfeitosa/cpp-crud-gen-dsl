//
// C++ Implementation: bwlist
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, BeanWeb Solutions (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwprimlist.h"
#include <iostream>

using namespace std;

BWPrimList::BWPrimList()
    : __BASIC_LIST()
{
 nullstr = "(null)";
}

BWPrimList::BWPrimList(const BWPrimList &l) : __BASIC_LIST(l)
{}

BWPrimList::BWPrimList(const __BASIC_LIST &l) : __BASIC_LIST(l)
{}

BWPrimList::~BWPrimList()
{}

/*!
    \fn BWPrimList::GetString(int index)
 */
BWPrimString& BWPrimList::GetString(size_type index) const
{
 if (index++ >= 0 && index <= size()) {
            return const_cast<BWPrimString&>(__BASIC_LIST::operator[](--index));
 }

 return const_cast<BWPrimString&>(nullstr);
}


/*!
    \fn BWList::GetInt(int index)
 */
int BWPrimList::GetInt(int index) const
{
  return GetString(index).GetInt();
}

/*!
    \fn BWPrimList::operator=(const BWPrimList &l)
 */
BWPrimList &BWPrimList::operator= (const BWPrimList &l)
{
  //std::cerr << "BWList: Copiando lista de "<<l.size()<<std::endl;
  return assign(l);
}

/*!
    \fn BWPrimList::operator=(const BWPrimList &l)
 */
BWPrimList &BWPrimList::operator= (const __BASIC_LIST &l)
{
  //std::cerr << "BWList: Copiando lista de "<<l.size()<<std::endl;
  return assign(l);
}

BWPrimList &BWPrimList::assign(const __BASIC_LIST &l)
{
  unsigned int i;
  this->clear();

  for (i = 0; i < l.size(); i++)
  {
    this->push_back(l[i]);
  }

  return *this;
}


/*!
    \fn BWPrimList::push_back(const BWPrimString &s)
 */
void BWPrimList::push_back(const BWPrimString &s)
{
 __BASIC_LIST::push_back(s);
}
