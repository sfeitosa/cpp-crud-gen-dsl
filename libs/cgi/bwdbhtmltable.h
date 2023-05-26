#ifndef CGIBWDBHTMLTABLE_H
#define CGIBWDBHTMLTABLE_H

#include <bwhtmltable.h>
#include <bwdatabase.h>

namespace CGI {

class BWDbHtmlTable : public CGI::BWHtmlTable
{
public:
    BWDbHtmlTable(BWConfig& cfg, BWHttp *channel, BWDb &db);
    BWDbHtmlTable(const BWString& cfg, BWHttp *channel, BWDb &db);

    ~BWDbHtmlTable();
    BWString GetInnerHTML();

protected:
    BWDb* bw_db;
protected:
    void prepareHtmlTable();
};

}

#endif
