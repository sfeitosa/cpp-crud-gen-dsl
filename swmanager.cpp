#include "swmanager.h"

int main(int argc, char **argv, char **envp) {
    BWLog::Severity sev;
    BWString session;

    g_log.SetLog(LOG_PATH);

    try {
        g_http = new BWHttp(envp);
    }
    catch (BWError &err) {
        delete g_http;
        exit(1);
    }

    try {
        g_config.Load(APP_CONFIG);
        g_db = bwdb_load(DB_CONFIG);
    }
    catch (BWError &err) {
    	*g_http << "Erro ao iniciar...<br>";
    	*g_http << err.GetErrorMessage();
        delete g_http;
        delete g_db;
        exit(1);
    }

    if (g_config["loglevel"] != "(null)" && g_config["loglevel"] != "") {
        sev = static_cast<BWLog::Severity>(g_config["loglevel"].GetInt());
        g_log.SetLogLevel(sev);
    }

    g_log(BWLog::MSG_XDEBUG) << "SWManager::Starting..." << bwlog::endl;
    
    g_http->SetHeader("Content-type", "text/html;charset=UTF-8");

    if (g_http->GetGETParameter("action") == "logout") {
        g_log(BWLog::MSG_XDEBUG) << "SWManager::action 'logout'..." << bwlog::endl;

        g_http->SetCookie("BWM_SESSION_ID", "(null)");
        loadLoginHtml();
        delete g_db;
        delete g_http;
        exit(0);
    }

    session = g_http->GetCookie("BWM_SESSION_ID");

    if (session == "" || session == "(null)") {

        g_log(BWLog::MSG_XDEBUG) << "SWManager::null session..." << bwlog::endl;

        try {
            loginHandler();
        }
        catch(BWError &e) {
            loadLoginHtml(e.GetErrorMessage());
            delete g_http;
            delete g_db;
            exit(1);
        }
    }
    else {
        try {
            if (checkSession(session)) {
                g_log(BWLog::MSG_XDEBUG) << "SWManager::valid session..." << bwlog::endl;
                try {
                    formHandler();
                }
                catch(BWError &e) {
                    errorHandler(e);
                    delete g_http;
                    delete g_db;
                    exit(1);
                }
            }
            else {
                loadLoginHtml("Sessão inválida.");
                g_http->SetCookie("BWM_SESSION_ID", "(null)");
            }
        }
        catch(BWError &e) {
            loadLoginHtml(e.GetErrorMessage());
            delete g_http;
            delete g_db;
            exit(1);
        }
    }

    g_db->Close();
    delete g_http;
    
    return 0;
}

void errorHandler(BWError &e) {
	BWTemplate alert;
	BWTemplate error;
	BWString msg = "<strong>Erro ao tratar requisição: </strong>";

	msg += e.GetErrorMessage();

	alert.FromFile(BW_HTML_ALERT);
	alert.SetAttribute("ALERT_CLASS", "alert-danger");
	alert.SetAttribute("MESSAGE", msg);

	error.FromFile(BW_HTML_ERROR);
	error.SetAttribute("CONTENT", alert.GetText());

	*g_http << error.GetText();
}

void loginHandler() {
    // Se o formulario nao estiver preenchido, devemos apresenta-lo ao usuario
    if (g_http->GetServerVar("REQUEST_METHOD") != "POST") {
        loadLoginHtml();
    }
    // Se estiver preenchido, realizar a autenticacao
    else {
        doLogin();
        loadManagerHtml();
    }
}

void formHandler() {
	BWAjaxRegContainer *reg;
	BWString crud;
	BWKeyPair vars;

    g_log(BWLog::MSG_XDEBUG) << "SWManager::formHandler" << bwlog::endl;

	// Se não vier de uma requisição POST, apresenta a tela
	// de boas vindas
	if (g_http->GetServerVar("REQUEST_METHOD") != "POST") {
        g_log(BWLog::MSG_XDEBUG) << "SWManager::formHandler -> showing welcome screen" << bwlog::endl;
	    loadManagerHtml();
	}
	// Se for uma requisição POST, apresenta o formulário
	// para o usuário
	else {

        g_log(BWLog::MSG_XDEBUG) << "SWManager::formHandler -> showing form" << bwlog::endl;

		vars = g_http->GetGetVars();
		vars.AppendKeyPair(g_http->GetPostVars());
		vars.AppendKeyPair(g_http->GetCookieVars());

        // Passando "BWC_USER_ID" com valor zero para não validar
        // permissões de usuário
        vars["BWC_USER_ID"] = "0";
		// Informacao do nome do banco de dados
		vars["__beanweb_dbname"] = "swm";

        g_log(BWLog::MSG_XDEBUG) << "SWManager::formHandler - vars(1) => " << vars.ToString() << bwlog::endl;

		if (g_config[vars["cad"] + ".form"] != "(null)") {
			crud = g_config[vars["cad"] + ".form"];
		}
		else {
			crud = g_config[vars["cad"] + ".file"];
		}

        g_log(BWLog::MSG_XDEBUG) << "SWManager::formHandler - vars(2) => " << vars.ToString() << bwlog::endl;

	    reg = new BWAjaxRegContainer(vars, crud, *g_db);

	    *g_http << reg->GetInnerHtml();
	}
}

void loadLoginHtml(const BWString &err) {
    BWTemplate header;
    BWTemplate form;
    BWTemplate scripts;
    BWTemplate alert;

    header.FromFile(BW_HTML_HEADER);
    header.SetAttribute("DESCRIPTION", "SWManager - Login");
    //header.SetAttribute("AUTHOR", "Anônimo");
    header.SetAttribute("SHORTCUT_ICON", "");
    header.SetAttribute("TITLE", "SWManager - Login");
    header.SetAttribute("CUSTOM_CSS", "/css/signin.css");

    scripts.FromFile(BW_HTML_SCRIPTS);
    scripts.SetAttribute("BW_CUSTOM_SCRIPT", "bwcmanager.js");

    form.FromFile(BW_HTML_LOGIN);
    form.SetAttribute("LOGO", "/img/logo.png");
    form.SetAttribute("FORM_ACTION", "/cgi-bin/swm.cgi");
    form.SetAttribute("BODY_SCRIPTS", scripts.GetText());

    if (err != "") {
        alert.FromFile(BW_HTML_ALERT);
        alert.SetAttribute("ALERT_CLASS", "alert-danger");
        alert.SetAttribute("MESSAGE", err);
        form.SetAttribute("FORM_ERROR", alert.GetText());
    }
    else {
        form.SetAttribute("FORM_ERROR", err);
    }

    *g_http << "<!DOCTYPE html>\n";
    *g_http << "<html lang=\"en\">\n";
    *g_http << header.GetText();
    *g_http << form.GetText();
    *g_http << "</html>\n";
}

void loadManagerHtml() {
	BWTemplate header;
	BWTemplate body;
	BWTemplate footer;
	BWTemplate scripts;
	BWTemplate welcome;
	BWHtmlNav menu(g_config);
	BWString link;

	header.FromFile(BW_HTML_HEADER);
	header.SetAttribute("DESCRIPTION", "Login - SWManager 0.1");
	header.SetAttribute("AUTHOR", "Anônimo");
	header.SetAttribute("SHORTCUT_ICON", "");
	header.SetAttribute("TITLE", "SWManager - Software Seller Manager");
	header.SetAttribute("CUSTOM_CSS", "/css/navbar.css");

	link = "<a href='http://github.com/sfeitosa/cppcrudgen'>CPP CRUD Generator &raquo;</a> ";

    footer.FromFile(BW_HTML_FOOTER);
    footer.SetAttribute("FOOTER_MSG", link);

    scripts.FromFile(BW_HTML_SCRIPTS);
    scripts.SetAttribute("BW_CUSTOM_SCRIPT", "bwcadmin.js");

    welcome.FromFile(BW_HTML_WELCOME);
    welcome.SetAttribute("MESSAGE1", "");
    welcome.SetAttribute("MESSAGE2", "");

	body.FromFile(BW_HTML_BODY);
	body.SetAttribute("BODY_MENU", menu.GetText());
	body.SetAttribute("BODY_DATA", welcome.GetText());
	body.SetAttribute("BODY_FOOTER", footer.GetText());
	body.SetAttribute("BODY_SCRIPTS", scripts.GetText());

	*g_http << "<!DOCTYPE html>\n";
	*g_http << "<html lang=\"en\">\n";
	*g_http << header.GetText();
	*g_http << body.GetText();
	*g_http << "</html>\n";
}

void doLogin() {
    BWString seed;
    BWString user;
    BWString pass;
    BWString session;
    BWString query;
    BWResultSet rset;

    seed = g_http->GetServerVar("UNIQUE_ID");
    user = g_http->GetPOSTParameter("usuario");
    pass = g_http->GetPOSTParameter("senha");

    query  = "SELECT id FROM swm_seller WHERE descr = '";
    query += user + "' AND password = '" + pass + "'";

    try {
        rset = g_db->ExecuteQuery(query);
    }
    catch (BWError &e) {
        throw BWError(APP_ERROR, "Erro ao validar usuário " + query);
    }

    if (rset.GetNumRows() <= 0) {
        throw BWError(APP_ERROR, "Usuário ou senha inválidos.");
    }

    session = generateSessionHash(seed, rset.GetString("id"));

    query  = "UPDATE swm_seller SET current_session = '" + session + "' ";
    query += "WHERE id = '" + rset.GetString("id") + "'";

    try {
        g_db->ExecuteUpdate(query);
    }
    catch (BWError &e) {
        throw BWError(APP_ERROR, "Erro ao gravar sessão.");
    }

    g_http->SetCookie("BWM_SESSION_ID", session);
    g_http->SetCookie("BWC_USER_ID", rset.GetString("id"));
    g_http->SetCookie("BWC_SEED_NR", seed);
}

bool checkSession(const BWString &session) {
    BWString query;
    BWResultSet rset;

    query  = "SELECT id FROM swm_seller WHERE id = '" + g_http->GetCookie("BWC_USER_ID") + "' ";
    query += "AND current_session = '" + session + "'";

    try {
        rset = g_db->ExecuteQuery(query);
    }
    catch (BWError &e) {
        throw BWError(APP_ERROR, "Erro ao consultar sessão.");
    }

    if (rset.GetNumRows() > 0) {
        if (session == generateSessionHash(g_http->GetCookie("BWC_SEED_NR"), g_http->GetCookie("BWC_USER_ID"))) {
            return true;
        }
    }

    return false;
}

BWString generateSessionHash(const BWString &seed, const BWString &user) {
    BWString session;

    session = "__B3@NW3B__" + seed + "*_$_*_12" + user + "98__";

    return session.Hash();
}
