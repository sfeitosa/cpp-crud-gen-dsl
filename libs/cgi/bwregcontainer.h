#ifndef CGIBWREGCONTAINER_H
#define CGIBWREGCONTAINER_H

#include <bwdatabase.h>
#include <bwdbhtmlform.h>
#include <bwdbhtmltable.h>
#include <bwhttp.h>
#include <bwconfig.h>
#include <iostream>
#include <bwkeypair.h>
#include <bwutils.h>

using namespace CGI;
using namespace std;

namespace CGI {


class BWRegContainer{
public:
    BWRegContainer(BWConfig &cfg, BWHttp *http, BWDb &db);
    BWRegContainer(const BWString &cfg, BWHttp *http, BWDb &db);
    void SetChannel(BWHttp *http);
    bool HasDataChange(void);

    ~BWRegContainer();

protected:
    void preParsing();
    void doEdit();
    void doDelete();
    void doEnable();
    void doList();
    void doMoveDown();
    void doMoveUp();
    void doDisable();
    void doAction(const BWString &action);
    void preForm();
    void preList();
    BWDbRelation getRelation(const BWString &);

    BWConfig bw_config;
    BWDb* bw_db;
    BWList rc_datafields;
    BWHttp* bw_http;
    BWDbHtmlForm* rc_form;
    BWDbHtmlTable* rc_table;
    BWString rc_tablename;
    bool has_data_change;
};

}

#endif
