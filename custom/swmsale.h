#include <bwdatabase.h>
#include <bwhttp.h>
#include <bwlog.h>
#include <iostream>
#include <bwutils.h>

using namespace std;
using namespace Http;
 
#define DB_CONFIG        "etc/swm.db"
#define BW_HTML_PANEL    "var/www/html/templates/panel.html"
#define BW_HTML_TABLE    "var/www/html/templates/table.html"
#define BW_HTML_MODAL    "var/www/html/templates/modal.html"
#define BW_HTML_FORM     "var/www/html/templates/form.html"
#define BW_HTML_BUTTON   "var/www/html/templates/button.html"
#define BW_HTML_ALERT    "var/www/html/templates/alert.html"
#define BW_HTML_ERROR    "var/www/html/templates/error.html"
#define BW_HTML_TEXT     "etc/fieldtypes/text.fft"
#define BW_HTML_READONLY "etc/fieldtypes/ReadOnly.fft"
#define BW_HTML_SELECT   "etc/fieldtypes/SelectList.fft"
#define BW_HTML_DATE     "etc/fieldtypes/Date.fft"

#define LOG_PATH "/tmp/swsale.log"

BWDb *g_db = NULL;
BWHttp *g_http = NULL;
BWConfig g_config;
BWLog g_log;

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
