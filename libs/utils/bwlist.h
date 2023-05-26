#ifndef BWPRIMLIST_H
#define BWPRIMLIST_H

#include <bwprimlist.h>
#include <bwstring.h>

class BWList : public BWPrimList
{
  public:
    BWList();
    BWList(const BWList &l);
    BWList(const BWPrimList &l);
    ~BWList();
    BWList & operator= (const BWList &l);
    BWList & operator= (const BWPrimList &l);
    BWString ToString(const BWString &sep=",", const BWString &pre="", const BWString &suf="");
    BWString ToSQLIn();
    BWString& operator[](size_type i);

  protected:
    BWList &assign(const BWPrimList &l);
};

#endif
