//
// C++ Interface: bwkeypair
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, BeanWeb Solutions (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWKEYPAIR_H
#define BWKEYPAIR_H

#include <bwstring.h>
#include <bwlist.h>

#ifdef USE_STL_MAP
#include <map>
typedef std::map<BWString, BWString> __BASIC_KEYPAIR;
#endif

/**
Classe basica implementando o modelo chave/valor

 @author
 */

class BWKeyPair : public __BASIC_KEYPAIR
{

    public:
        BWKeyPair();
        ~BWKeyPair();

        BWString GetString(const BWString &idx);
        int GetInt(const BWString &idx);
        int GetInt(int index);
        BWString GetString(int index);

        BWString ToString(const BWString &kvsep = "=", 
                const BWString &regsep = " ", const BWString &valuedelim = "'",
                const BWString &keydelim = "");

        void FromString(const BWString &str, const BWString &kpsep = "=",
                const BWString &regsep = " ", const BWString &valuedelim = "'",
                const BWString &keydelim = "");

        BWString ToSQLWhere();
        BWString ToSQLInsert();
        BWString& operator[](const BWString &s);
        BWKeyPair& AppendKeyPair(BWKeyPair &kpr);
        BWKeyPair GetSubKeyPair(BWList &l);
        void ToFile(const BWString &fname, const BWString &kvsep = ":",
                const BWString &regsep = "\n", const BWString &valuedelim = "",
                const BWString &keydelim = "");

    protected:
        void kpInsert(const BWString &str, const BWString &kpsep = "=",
                const BWString &valuedelim = "'", const BWString &keydelim = "");

};

#endif
