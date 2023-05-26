#include <bwdatabase.h>
#include <bwhttp.h>
#include <bwlog.h>
#include <bwhtmlnav.h>
#include <bwajaxregcontainer.h>
#include <iostream>
#include <bwutils.h>

using namespace std;
using namespace CGI;
using namespace Http;

#define DB_CONFIG "etc/swm.db"
#define APP_CONFIG "etc/swm.conf"
#define LOG_PATH "/tmp/swmanager.log"

#define BW_HTML_HEADER "var/www/html/templates/header.html"
#define BW_HTML_BODY "var/www/html/templates/body.html"
#define BW_HTML_FOOTER "var/www/html/templates/footer.html"
#define BW_HTML_SCRIPTS "var/www/html/templates/scripts.html"
#define BW_HTML_WELCOME "var/www/html/templates/welcome.html"
#define BW_HTML_ALERT "var/www/html/templates/alert.html"
#define BW_HTML_ERROR "var/www/html/templates/error.html"
#define BW_HTML_LOGIN "var/www/html/templates/form_login.html"

BWDb *g_db = NULL;
BWHttp *g_http = NULL;
BWConfig g_config;
BWLog g_log;

void loginHandler();
void formHandler();
void errorHandler(BWError &e);
void loadLoginHtml(const BWString &err = "");
void loadManagerHtml();
bool checkSession(const BWString &);
void doLogin();
BWString generateSessionHash(const BWString &, const BWString &);
