#include "bwkeypair.h"
#include <fstream>

using std::fstream;
using std::flush;

BWKeyPair::BWKeyPair()
: __BASIC_KEYPAIR()
{}


BWKeyPair::~BWKeyPair()
{}




/*!
    \fn BWKeyPair::GetString(const BWString &idx)
 */
BWString BWKeyPair::GetString(const BWString &idx)
{
    return operator[](idx);
}


/*!
    \fn BWKeyPair::GetInt(const BWString &idx)
 */
int BWKeyPair::GetInt(const BWString &idx)
{
    return operator[](idx).GetInt();
}


/*!
    \fn BWKeyPair::GetInt(int index)
 */
int BWKeyPair::GetInt(int index)
{
    return GetString(index).GetInt();
}


/*!
    \fn BWKeyPair::GetString(int index)
 */
BWString BWKeyPair::GetString(int index)
{
    char idx[15];
    sprintf(idx, "%d", index);
    return operator[](idx);
}


/*!
    \fn BWKeyPair::operator[](int index)

BWString BWKeyPair::operator[](int index)
{
    return GetString(index);
}
 */



/*!
    \fn BWKeyPair::ToString(const BWString &kvsep=, const BWString &regsep, const BWString &valuedelim, const BWString &keydelim)
 */
BWString BWKeyPair::ToString(const BWString &kvsep, const BWString &regsep,
        const BWString &valuedelim, const BWString &keydelim)
{
    BWKeyPair::iterator itr = begin();
    BWString ret;

    ret = keydelim + itr->first + keydelim + kvsep;

    if (itr->second == "<%#BWDB_NULL_VALUE#%>") {
        ret += "NULL";
    }
    else {
        ret += valuedelim + itr->second + valuedelim;
    }

    while (++itr != end())
    {
        ret += regsep;
        ret += keydelim + itr->first + keydelim;
        ret += kvsep;
        if (itr->second == "<%#BWDB_NULL_VALUE#%>") {
            ret += "NULL";
        }
        else {
            ret += valuedelim;
            if (itr->second == "(null)") {
                ret += valuedelim;
            }
            else {
                ret += itr->second + valuedelim;
            }
        }
    }

    return ret;
}

void BWKeyPair::FromString(const BWString &str, const BWString &kpsep,
        const BWString &regsep, const BWString &valuedelim,
        const BWString &keydelim)
{

    int epos = 0;
    int mpos = 0;
    BWString strIn = str;
    BWString tmp;

    do {
        mpos = strIn.find(keydelim + kpsep + valuedelim);
        epos = strIn.find(valuedelim + regsep + keydelim,
                mpos + kpsep.size() + valuedelim.size());

        if (epos > 0) {
            tmp = strIn.substr(0, epos + valuedelim.size());
            strIn.erase(0, epos + valuedelim.size() + regsep.size());
            kpInsert(tmp, kpsep, valuedelim, keydelim);
        }
        else {
            kpInsert(strIn, kpsep, valuedelim, keydelim);
        }
    } while (epos > 0);
}

void BWKeyPair::kpInsert(const BWString &str, const BWString &kpsep,
        const BWString &valuedelim, const BWString &keydelim) {
    int pos = 0;
    BWString strnew = str;
    BWString key;
    BWString pair;

    pos = str.find(keydelim + kpsep + valuedelim);

    key = str.substr(keydelim.size(), pos - keydelim.size());
    strnew.erase(0, pos + keydelim.size() + kpsep.size() + valuedelim.size());

    if (valuedelim.length() > 0) {
        pos = strnew.find(valuedelim);
        pair = strnew.substr(0, pos);
    }
    else {
        pair = strnew;
    }

    operator[](key) = pair;
}


/*!
    \fn BWKeyPair::ToSQLWhere()
 */
BWString BWKeyPair::ToSQLWhere()
{
    return ToString("=", " AND ");
}

/*!
    \fn BWKeyPair::ToSQLWhere()
 */
BWString BWKeyPair::ToSQLInsert()
{
    BWKeyPair::iterator itr = begin();
    BWString keys = "(";
    BWString values = "(";

    keys += itr->first;
    values += "'" + itr->second + "'";

    while (++itr != end()) {
        keys += "," + itr->first;
        values += ",'" + itr->second + "'";
    }
    keys += ")";
    values += ")";

    return keys + " VALUES " + values + ";";
}

/*!
    \fn BWKeyPair::operator[](const BWString &s)
 */
BWString& BWKeyPair::operator[](const BWString &s)
{
    BWKeyPair::iterator itr = this->begin();
    itr = this->find(s);
    if (itr == this->end()) {
        __BASIC_KEYPAIR::operator[](s) = "(null)";
    }
    return __BASIC_KEYPAIR::operator[](s);
}



/*!
    \fn BWKeyPair::AppendKeyPair(BWKeyPair &kpr)
 */
BWKeyPair& BWKeyPair::AppendKeyPair(BWKeyPair &kpr)
{
    BWKeyPair::iterator itr;

    itr = kpr.begin();
    while (itr != kpr.end()) {
        operator[](itr->first) = itr->second;
        itr++;
    }

    return *this;
}


/*!
    \fn BWKeyPair::GetSubKeyPair(BWList &l)
 */
BWKeyPair BWKeyPair::GetSubKeyPair(BWList &l)
{
    unsigned int i;
    BWKeyPair ret;

    for (i = 0; i < l.size(); i++) {
        if (l[i] != "(null)") {
            ret[l[i]] = operator[](l[i]);
        }
    }

    return ret;
}


/*!
    \fn BWKeyPair::ToFile(const BWString &fname, const BWString &kvsep, const BWString &regsep, 
	const BWString &valuedelim, const BWString &keydelim))
 */
void BWKeyPair::ToFile(const BWString &fname, const BWString &kvsep,
        const BWString &regsep, const BWString &valuedelim,
        const BWString &keydelim)
{
    BWString tmp;
    fstream file(fname.c_str(), fstream::out);

    tmp = ToString(kvsep, regsep, valuedelim, keydelim);

    file << tmp << flush;
}
