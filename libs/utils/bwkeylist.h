#ifndef BWKEYLIST_H
#define BWKEYLIST_H

#include <bwstring.h>
#include <bwlist.h>

#ifdef USE_STL_MAP
#include <map>
typedef  std::map<BWString, BWList> __BASIC_KEYLIST;
#endif

class BWKeyList : public __BASIC_KEYLIST
{
  public:
    BWKeyList();
    ~BWKeyList();
    BWString GetString(const BWString &fld, int idx);
    int GetInt(const BWString &fld, int idx);

};

#endif
