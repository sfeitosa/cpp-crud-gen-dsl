#include "swmsale.h"

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

    g_http->SetHeader("Content-type", "text/html;charset=UTF-8");

    session = g_http->GetCookie("BWM_SESSION_ID");

    try {
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

    try {
        formHandler();
    }
    catch(BWError &e) {
        errorHandler(e);

        if (g_db->AtTransaction()) {
            g_db->Rollback();
        }

        delete g_http;
        delete g_db;
        exit(1);
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

void formHandler() {
    if (g_http->GetGETParameter("action") == "list") {
        loadHtmlPanel();
    }
    else if (g_http->GetGETParameter("action") == "modal") {
        loadHtmlModal();
    }
    else if (g_http->GetGETParameter("action") == "save") {
        //saveFormData();
        //loadHtmlPanel("Venda efetuada com sucesso.");
        loadHtmlPanel("Operation under construction...");
    }
    else {
        throw BWError(APP_ERROR, "Função inválida.");
    }
}

void saveFormData() {
    BWString query;
    BWString cmd;
    BWResultSet rset;
    BWKeyPair rec;
    BWString msg;

    rec = g_http->GetPostVars();

    if (rec["cliente_id"] == "(null)") {
        throw BWError(APP_ERROR, "Cliente inválido ou não informado.");
    }

    if (rec["software_id"] == "(null)") {
        throw BWError(APP_ERROR, "Licença inválida ou não informada.");
    }

    if (rec["quantidade"] == "(null)") {
        throw BWError(APP_ERROR, "Quantidade inválida ou não informada.");
    }

    // Baixar licença vendida
    query = "SELECT software_licenca_id, quantidade FROM swm_estoque WHERE id = ";
    query += rec["software_licenca_id"];

    rset = g_db->ExecuteQuery(query);

    if (rset.GetNumRows() <= 0) {
        throw BWError(APP_ERROR, "Licença de Software inválida!");
    }

    g_db->BeginTransaction();

    if (rec["quantidade"].GetInt() > rset.GetInt("quantidade")) {
        throw BWError(APP_ERROR, "Quantidade de licenças solicitadas indisponível.");
    }

    cmd = "UPDATE swm_estoque SET quantidade = ";
    cmd.AppendInt(rset.GetInt("quantidade") - rec["quantidade"].GetInt());
    cmd += " WHERE id = " + rec["software_licenca_id"];

    g_db->ExecuteUpdate(cmd);

    // Remover registro de crédito se não tiver mais licenças disponíveis
    if ((rset.GetInt("quantidade") - rec["quantidade"].GetInt()) == 0) {
        cmd = "DELETE FROM swm_estoque WHERE id = " + rec["software_licenca_id"];
        g_db->ExecuteUpdate(cmd);
    }

    // Inserir informações da venda recém efetuada
    rec["revenda_id"] = g_http->GetCookie("BWC_USER_ID");
    rec["software_licenca_id"] = rset.GetString("software_licenca_id");

    g_db->Insert("swm_venda", rec);

    g_db->Commit();
}

void loadHtmlPanel(const BWString &msg) {
    BWTemplate panel;
    BWTemplate button;
    BWTemplate alert;
    BWString info = "";

    alert.FromFile(BW_HTML_ALERT);
    alert.SetAttribute("ALERT_CLASS", "alert-success");
    alert.SetAttribute("MESSAGE", msg);

    if (msg != "") {
        info += "<div id='bw-dialog-message' class='fade'>" + alert.GetText() + "</div>\n";
    }
    info += "Here you can link your licenses to the customers.";

    button.FromFile(BW_HTML_BUTTON);
    button.SetAttribute("HREF", "/cgi-bin/swmsale.cgi?action=modal");
    button.SetAttribute("CLASS", "btn-primary bwmodal");
    button.SetAttribute("TEXT", "<span class='glyphicon glyphicon-plus'></span> New Sale");

    panel.FromFile(BW_HTML_PANEL);

    panel.SetAttribute("TITLE", "Sales");
    panel.SetAttribute("INFO", info);
    panel.SetAttribute("CONTENT", getHtmlTable());
    panel.SetAttribute("OPTIONS", button.GetText());

    *g_http << panel.GetText();
}

BWString getHtmlTable() {
    BWResultSet rset;
    BWString query = "SELECT swm_sale.id, swm_seller.descr AS revenda, swm_individualcustomer.descr AS cliente, swm_software_license.descr AS software, ";
    BWTemplate table;
    BWString theader;
    BWString tbody;
    BWList rev, cli, sfw, vnd, qtd;

    query += "saledate, quantity FROM swm_sale ";
    query += "INNER JOIN swm_seller ON swm_seller.id = swm_sale.seller_id ";
    query += "INNER JOIN swm_individualcustomer ON swm_individualcustomer.id = swm_sale.individualcustomer_id ";
    query += "INNER JOIN swm_software_license ON swm_software_license.id = swm_sale.software_license_id";
    query += " ORDER BY saledate DESC";

    cerr << query << endl;

    try {
        rset = g_db->ExecuteQuery(query);
    }
    catch (BWError &e) {
        throw BWError(APP_ERROR, "Erro ao consultar informações.");
    }

    table.FromFile(BW_HTML_TABLE);

    theader = "<th>Seller</th><th>Costumer</th><th>Software</th><th>Quantity</th><th>Sale Date</th>";
    tbody = "<td><%#revenda#%></td><td><%#cliente#%></td><td><%#software#%></td><td><%#quantidade#%></td><td><%#datahora#%></td>";

    table.SetAttribute("HEADER", theader);
    table.ReplaceText("BODY", tbody);

    if (rset.GetNumRows() > 0) {
        rev = rset.GetColumn("revenda");
        cli = rset.GetColumn("cliente");
        sfw = rset.GetColumn("software");
        qtd = rset.GetColumn("quantidade");
        vnd = rset.GetColumn("datahora");
        table.SetAttributeList("revenda", rev);
        table.SetAttributeList("cliente", cli);
        table.SetAttributeList("software", sfw);
        table.SetAttributeList("quantidade", qtd);
        table.SetAttributeList("datahora", vnd);

    }

    return table.GetText();
}

void loadHtmlModal() {
    BWTemplate modal;
    BWTemplate form;
    BWString html;

    modal.FromFile(BW_HTML_MODAL);
    form.FromFile(BW_HTML_FORM);

    modal.SetAttribute("TITLE", "Sale");

    html  = getHtmlRevenda();
    html += getHtmlCliente();
    html += getHtmlSoftware();
    html += getHtmlQuantidade();

    form.SetAttribute("CONTENT", html);

    form.SetAttribute("FORM_NAME", "venda");
    form.SetAttribute("FORM_ACTION", "/cgi-bin/swmsale.cgi?action=save");

    modal.SetAttribute("CONTENT", form.GetText());

    modal.SetAttribute("BUTTON_OK", "Finish Sale");

    *g_http << modal.GetText();
}

BWString getHtmlRevenda() {
    BWString query = "SELECT descr FROM swm_seller WHERE id = ";
    BWResultSet rset;
    BWTemplate tpl;
    BWList ids, descrs;
    
    query += g_http->GetCookie("BWC_USER_ID");

    rset = g_db->ExecuteQuery(query);

    tpl.FromFile(BW_HTML_READONLY);

    tpl.SetAttribute("FIELD_LABEL", "Seller");
    tpl.SetAttribute("FIELD_SUBLABEL", "");
    tpl.SetAttribute("FIELD_HELPER", "");
    tpl.SetAttribute("FIELD_NAME", "seller_id");
    tpl.SetAttribute("FIELD_VALUE", rset.GetString("descr"));
    tpl.SetAttribute("FIELD_ICON", "<span class='glyphicon glyphicon-user'></span>");

    return tpl.GetText();
}

BWString getHtmlCliente() {
    BWString query;
    BWResultSet rset;
    BWTemplate tpl;
    BWList ids, descrs;

    query  = "SELECT id, descr FROM swm_individualcustomer";

    rset = g_db->ExecuteQuery(query);

    tpl.FromFile(BW_HTML_SELECT);

    tpl.SetAttribute("FIELD_TYPE", "SelectList");
    tpl.SetAttribute("FIELD_LABEL", "Customer");
    tpl.SetAttribute("FIELD_SUBLABEL", "");
    tpl.SetAttribute("FIELD_HELPER", "");
    tpl.SetAttribute("FIELD_NAME", "individualcustomer_id");

    if (rset.GetNumRows() > 0) {
        ids = rset.GetColumn("id");
        descrs = rset.GetColumn("descr");
        tpl.SetAttributeList("FIELDOPT_ID", ids);
        tpl.SetAttributeList("FIELDOPT_DESCR", descrs);
    }

    return tpl.GetText();
}

BWString getHtmlSoftware() {
    BWString query;
    BWResultSet rset;
    BWTemplate tpl;
    BWList ids, descrs;

    // query  = "SELECT swm_estoque.id, software_licenca_id, swm_software_licenca.descr || ' - ' ||  quantidade || ' Disponíveis' AS software ";
    // query += "FROM swm_estoque INNER JOIN swm_software_licenca ";
    // query += "ON swm_software_licenca.id = swm_estoque.software_licenca_id ";
    // query += "WHERE swm_estoque.revenda_id = " + g_http->GetCookie("BWC_USER_ID");

    query  = "SELECT id, descr FROM swm_software";

    rset = g_db->ExecuteQuery(query);

    tpl.FromFile(BW_HTML_SELECT);

    tpl.SetAttribute("FIELD_TYPE", "SelectList");
    tpl.SetAttribute("FIELD_LABEL", "Software");
    tpl.SetAttribute("FIELD_SUBLABEL", "");
    tpl.SetAttribute("FIELD_HELPER", "");
    tpl.SetAttribute("FIELD_NAME", "software_id");

    if (rset.GetNumRows() > 0) {
        ids = rset.GetColumn("id");
        descrs = rset.GetColumn("descr");
        tpl.SetAttributeList("FIELDOPT_ID", ids);
        tpl.SetAttributeList("FIELDOPT_DESCR", descrs);
    }

    return tpl.GetText();
}

BWString getHtmlQuantidade() {
    BWTemplate tpl;

    tpl.FromFile(BW_HTML_TEXT);

    tpl.SetAttribute("FIELD_TYPE", "text");
    tpl.SetAttribute("FIELD_LABEL", "Quantity");
    tpl.SetAttribute("FIELD_SUBLABEL", "");
    tpl.SetAttribute("FIELD_HELPER", "");
    tpl.SetAttribute("FIELD_NAME", "quantity");
    tpl.SetAttribute("FIELD_VALUE", "");
    tpl.SetAttribute("FIELD_ICON", "<span class='glyphicon glyphicon-shopping-cart'></span>");

    return tpl.GetText();
}

