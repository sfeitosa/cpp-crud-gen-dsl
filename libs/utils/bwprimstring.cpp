#include "bwprimstring.h"
#include <iostream>


BWPrimString::BWPrimString() : __BASIC_STRING(__BASIC_STRING(""))
{}

BWPrimString::BWPrimString(const BWPrimString &s) : __BASIC_STRING(s)
{}

BWPrimString::BWPrimString(const char *s) : __BASIC_STRING(s)
{}

BWPrimString::BWPrimString(const __BASIC_STRING &s) : __BASIC_STRING(s)
{}


BWPrimString::~BWPrimString()
{}


/*!
    @fn BWString::GetInt()
    @brief Retorna inteiro representado pela string
 */
int BWPrimString::GetInt() const
{
  return atoi(this[0].c_str());
}


/*!
    \fn BWPrimString::operator=(const BWPrimString &s)
 */
BWPrimString &BWPrimString::operator= (const BWPrimString &s)
{
  //std::cerr << "BWPrimString: Copiando "<<s<<std::endl;
  return static_cast<BWPrimString&>(this->assign(s));
}

/*!
    \fn BWString::AppendInt(int i)
 */
BWPrimString& BWPrimString::AppendInt(int i)
{
  char ints[30];
  sprintf(ints, "%d", i);
  *this += ints;

  return this[0];
}
