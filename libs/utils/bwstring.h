//
// C++ Interface: bwstring
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, BeanWeb (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWSTRING_H
#define BWSTRING_H

#include <bwprimstring.h>
#include <bwerror.h>
#include <bwprimlist.h>
// #include <openssl/md5.h>
#include <openssl/evp.h>
#include <fstream>
#include <iomanip>
#include <sstream>

/**
Classe de manipulacao basica de string

 @author Bruno Moreira Guedes
*/
class BWString : public BWPrimString
{
  public:
    BWString();
    BWString(const char *s);
    BWString(const BWString &s);
    BWString(const BWPrimString &s);
    BWString(const __BASIC_STRING &s);
    ~BWString();
    BWPrimList Split(const BWPrimString &sep, int limit=0) const;
    BWString &operator= (const BWString &s);
    BWString Hash();
    void Replace(const BWString &s1, const BWString &s2, int limit = 0);
    BWString& ToUpper();
    BWString& ToLower();
    BWString &Escape(const BWString &str = "'", const BWString &esc = "\\");
    BWString &FromFile(const BWString &fname);
    BWString &LFilter(const BWString &f);
    BWString &RFilter(const BWString &f);
    BWString &RTrim();
    BWString &LTrim();
    BWString &Trim();
    BWString &LPad(int len, char chr = '0');
    void ToFile(const BWString &);

};

#endif
