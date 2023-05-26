//
// C++ Interface: swmsale
//
// Description: 
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2014
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <bwdatabase.h>
#include <bwhttp.h>
#include <bwlog.h>
#include <iostream>
#include <bwutils.h>

using namespace std;
using namespace Http;
 
#define DB_CONFIG        "/srv/mestrado/sbd/swmanager/etc/swm.db"
#define BW_HTML_PANEL    "/srv/mestrado/sbd/swmanager/var/www/html/templates/panel.html"
#define BW_HTML_TABLE    "/srv/mestrado/sbd/swmanager/var/www/html/templates/table.html"
#define BW_HTML_MODAL    "/srv/mestrado/sbd/swmanager/var/www/html/templates/modal.html"
#define BW_HTML_FORM     "/srv/mestrado/sbd/swmanager/var/www/html/templates/form.html"
#define BW_HTML_BUTTON   "/srv/mestrado/sbd/swmanager/var/www/html/templates/button.html"
#define BW_HTML_ALERT    "/srv/mestrado/sbd/swmanager/var/www/html/templates/alert.html"
#define BW_HTML_ERROR    "/srv/mestrado/sbd/swmanager/var/www/html/templates/error.html"
#define BW_HTML_TEXT "/srv/mestrado/sbd/swmanager/etc/fieldtypes/text.fft"
#define BW_HTML_READONLY "/srv/mestrado/sbd/swmanager/etc/fieldtypes/ReadOnly.fft"
#define BW_HTML_SELECT   "/srv/mestrado/sbd/swmanager/etc/fieldtypes/SelectList.fft"
#define BW_HTML_DATE     "/srv/mestrado/sbd/swmanager/etc/fieldtypes/Date.fft"

BWDb *g_db = NULL;
BWHttp *g_http = NULL;
BWConfig g_config;
BWLog g_log("/tmp/swsale.log");

void formHandler();
void errorHandler(BWError &e);
void loadHtmlPanel(const BWString &msg = "");
BWString getHtmlTable();
BWString getHtmlRevenda();
BWString getHtmlCliente();
BWString getHtmlSoftware();
BWString getHtmlQuantidade();
void saveFormData();
void loadHtmlModal();
