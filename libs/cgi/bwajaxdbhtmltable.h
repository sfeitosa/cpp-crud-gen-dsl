#ifndef CGIBWAJAXDBHTMLTABLE_H
#define CGIBWAJAXDBHTMLTABLE_H

#include <bwajaxhtmltable.h>
#include <bwdatabase.h>
#include <cmath>
#include <bwaccessprofile.h>

namespace CGI {

class BWAjaxDbHtmlTable : public CGI::BWAjaxHtmlTable
{
public:
    BWAjaxDbHtmlTable(BWKeyPair &, BWConfig &cfg, BWDb &db);
    BWAjaxDbHtmlTable(BWKeyPair &, const BWString &cfg, BWDb &db);

    ~BWAjaxDbHtmlTable();
    BWString GetInnerHTML();

protected:
    BWDb* m_db;
protected:
    void prepareHtmlTable();
    BWString generateQuery(bool justowner = false);
    BWString getTotalPages();
};

}

#endif
