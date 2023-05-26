//
// C++ Interface: bwkeylist
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, BeanWeb Solutions (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWKEYLIST_H
#define BWKEYLIST_H

#include <bwstring.h>
#include <bwlist.h>

#ifdef USE_STL_MAP
#include <map>
typedef  std::map<BWString, BWList> __BASIC_KEYLIST;
#endif
/**
Classe para manipulacao de listas indexadas por chave

 @author Bruno Moreira Guedes <bruno@beanweb.com.br>
*/
class BWKeyList : public __BASIC_KEYLIST
{
  public:
    BWKeyList();
    ~BWKeyList();
    BWString GetString(const BWString &fld, int idx);
    int GetInt(const BWString &fld, int idx);

};

#endif
