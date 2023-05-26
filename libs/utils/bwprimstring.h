#ifndef BWPRIMSTRING_H
#define BWPRIMSTRING_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef USE_STL_STRING
#include <string>
typedef std::string __BASIC_STRING;
#endif

/**
String primitiva

 @author
*/
class BWPrimString : public __BASIC_STRING
{
  public:
    BWPrimString();
    BWPrimString(const BWPrimString &s);
    BWPrimString(const char *s);
    BWPrimString(const __BASIC_STRING &s);
    ~BWPrimString();
    int GetInt() const;
    BWPrimString &operator= (const BWPrimString &s);
    BWPrimString& AppendInt(int i);
};

#endif
