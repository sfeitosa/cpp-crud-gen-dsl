#ifndef CGIBWAJAXREGCONTAINER_H
#define CGIBWAJAXREGCONTAINER_H

#include <bwdatabase.h>
#include <bwajaxdbhtmlform.h>
#include <bwajaxdbhtmltable.h>
#include <bwaccessprofile.h>
#include <bwformactionfactory.h>
#include <bwhttp.h>
#include <bwconfig.h>
#include <iostream>
#include <bwkeypair.h>
#include <bwutils.h>

#define _BW_HTML_PANEL "var/www/html/templates/panel.html"
#define _BW_HTML_BUTTON "var/www/html/templates/button.html"
#define _BW_HTML_ALERT "var/www/html/templates/alert.html"

using namespace CGI;
using namespace std;

namespace CGI {

class BWAjaxRegContainer{
public:
    BWAjaxRegContainer(BWKeyPair &data, const BWString &crud, BWDb &db);
    BWString GetInnerHtml();
    bool HasDataChange(void);

    ~BWAjaxRegContainer();

protected:
    void preParsing();

    BWString doEdit(const BWString &);
    BWString doDelete();
    /*
    void doForceDelete();
    void doEnable();
    */
    BWString doList(const BWString &msg = "");
    BWString doMoveDown();
    BWString doMoveUp();
    /*
    void doDisable();
    */
    BWString executeAction(const BWString &);
    void preForm(const BWString &action);
    BWDbRelation getRelation(const BWString &);
    BWString getOptions(const BWString &);
    BWString generateMessage(const BWString &, const BWString &);
    void doFormAction(const BWString &);
    bool matchCreator(const BWString &id);
    void setLastUpdate();

    BWKeyPair m_data;
    BWConfig m_crud;
    BWDb* m_db;
    BWList m_datafields;
    BWAjaxDbHtmlForm *m_form;
    BWAjaxDbHtmlTable *m_table;
    BWString m_tablename;
    BWLog m_log;
    //bool has_data_change;
};

}

#endif
