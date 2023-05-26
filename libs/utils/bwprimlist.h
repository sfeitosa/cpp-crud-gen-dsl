//
// C++ Interface: bwlist
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, BeanWeb Solutions (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWLIST_H
#define BWLIST_H

#include <bwprimstring.h>

#ifdef USE_STL_VECTOR
#include <vector>
typedef std::vector<BWPrimString> __BASIC_LIST;
#endif
/**
Abstracao e manipulacao de listas

 @author
*/
class BWPrimList : public __BASIC_LIST
{
  public:
    BWPrimList();
    BWPrimList(const BWPrimList &l);
    BWPrimList(const __BASIC_LIST &l);
    ~BWPrimList();
    BWPrimString& GetString(size_type index) const;
    int GetInt(int index) const;

    BWPrimList& operator= (const BWPrimList &l);
    BWPrimList& operator= (const __BASIC_LIST &l);
    void push_back(const BWPrimString &s);
    BWPrimString nullstr;

  protected:
    BWPrimList &assign(const __BASIC_LIST &l);
};

#endif
