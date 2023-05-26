#include "swmsale.h"

int main(int argc, char **argv, char **envp) {
    BWLog::Severity sev;
    BWString session;

    try {
        g_http = new BWHttp(envp);
    }
    catch (BWError &err) {
        delete g_http;
        exit(1);
    }

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
    
    g_http->SetHeader("Content-type", "text/html;charset=UTF-8");

    session = g_http->GetCookie("BWM_SESSION_ID");

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
        saveFormData();
        loadHtmlPanel("Venda efetuada com sucesso.");
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

    if (rec["software_licenca_id"] == "(null)") {
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
    info += "Aqui você pode associar suas licenças para os clientes.";

    button.FromFile(BW_HTML_BUTTON);
    button.SetAttribute("HREF", "/sbd-bin/swmsale.cgi?action=modal");
    button.SetAttribute("CLASS", "btn-primary bwmodal");
    button.SetAttribute("TEXT", "<span class='glyphicon glyphicon-plus'></span> Nova Venda");

    panel.FromFile(BW_HTML_PANEL);

    panel.SetAttribute("TITLE", "Vendas");
    panel.SetAttribute("INFO", info);
    panel.SetAttribute("CONTENT", getHtmlTable());
    panel.SetAttribute("OPTIONS", button.GetText());

    *g_http << panel.GetText();
}

BWString getHtmlTable() {
    BWResultSet rset;
    BWString query = "SELECT swm_venda.id, swm_revenda.descr AS revenda, swm_cliente.descr AS cliente, swm_software_licenca.descr AS software, ";
    BWTemplate table;
    BWString theader;
    BWString tbody;
    BWList rev, cli, sfw, vnd, qtd;

    query += "datahora, quantidade FROM swm_venda ";
    query += "INNER JOIN swm_revenda ON swm_revenda.id = swm_venda.revenda_id ";
    query += "INNER JOIN swm_cliente ON swm_cliente.id = swm_venda.cliente_id ";
    query += "INNER JOIN swm_software_licenca ON swm_software_licenca.id = swm_venda.software_licenca_id";
    query += " ORDER BY datahora DESC";

    try {
        rset = g_db->ExecuteQuery(query);
    }
    catch (BWError &e) {
        throw BWError(APP_ERROR, "Erro ao consultar informações.");
    }

    table.FromFile(BW_HTML_TABLE);

    theader = "<th>Revenda</th><th>Cliente</th><th>Software</th><th>Quantidade</th><th>Data da Venda</th>";
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

    modal.SetAttribute("TITLE", "Venda");

    html  = getHtmlRevenda();
    html += getHtmlCliente();
    html += getHtmlSoftware();
    html += getHtmlQuantidade();

    form.SetAttribute("CONTENT", html);

    form.SetAttribute("FORM_NAME", "venda");
    form.SetAttribute("FORM_ACTION", "/sbd-bin/swmsale.cgi?action=save");

    modal.SetAttribute("CONTENT", form.GetText());

    modal.SetAttribute("BUTTON_OK", "Concluir Venda");

    *g_http << modal.GetText();
}

BWString getHtmlRevenda() {
    BWString query = "SELECT descr FROM swm_revenda WHERE id = ";
    BWResultSet rset;
    BWTemplate tpl;
    BWList ids, descrs;
    
    query += g_http->GetCookie("BWC_USER_ID");

    rset = g_db->ExecuteQuery(query);

    tpl.FromFile(BW_HTML_READONLY);

    tpl.SetAttribute("FIELD_LABEL", "Revenda");
    tpl.SetAttribute("FIELD_SUBLABEL", "");
    tpl.SetAttribute("FIELD_HELPER", "");
    tpl.SetAttribute("FIELD_NAME", "revenda_id");
    tpl.SetAttribute("FIELD_VALUE", rset.GetString("descr"));
    tpl.SetAttribute("FIELD_ICON", "<span class='glyphicon glyphicon-user'></span>");

    return tpl.GetText();
}

BWString getHtmlCliente() {
    BWString query;
    BWResultSet rset;
    BWTemplate tpl;
    BWList ids, descrs;

    query  = "SELECT id, descr FROM swm_cliente";

    rset = g_db->ExecuteQuery(query);

    tpl.FromFile(BW_HTML_SELECT);

    tpl.SetAttribute("FIELD_TYPE", "SelectList");
    tpl.SetAttribute("FIELD_LABEL", "Cliente");
    tpl.SetAttribute("FIELD_SUBLABEL", "");
    tpl.SetAttribute("FIELD_HELPER", "");
    tpl.SetAttribute("FIELD_NAME", "cliente_id");

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

    query  = "SELECT swm_estoque.id, software_licenca_id, swm_software_licenca.descr || ' - ' ||  quantidade || ' Disponíveis' AS software ";
    query += "FROM swm_estoque INNER JOIN swm_software_licenca ";
    query += "ON swm_software_licenca.id = swm_estoque.software_licenca_id ";
    query += "WHERE swm_estoque.revenda_id = " + g_http->GetCookie("BWC_USER_ID");

    rset = g_db->ExecuteQuery(query);

    tpl.FromFile(BW_HTML_SELECT);

    tpl.SetAttribute("FIELD_TYPE", "SelectList");
    tpl.SetAttribute("FIELD_LABEL", "Software");
    tpl.SetAttribute("FIELD_SUBLABEL", "");
    tpl.SetAttribute("FIELD_HELPER", "");
    tpl.SetAttribute("FIELD_NAME", "software_licenca_id");

    if (rset.GetNumRows() > 0) {
        ids = rset.GetColumn("id");
        descrs = rset.GetColumn("software");
        tpl.SetAttributeList("FIELDOPT_ID", ids);
        tpl.SetAttributeList("FIELDOPT_DESCR", descrs);
    }

    return tpl.GetText();
}

BWString getHtmlQuantidade() {
    BWTemplate tpl;

    tpl.FromFile(BW_HTML_TEXT);

    tpl.SetAttribute("FIELD_TYPE", "text");
    tpl.SetAttribute("FIELD_LABEL", "Quantidade");
    tpl.SetAttribute("FIELD_SUBLABEL", "");
    tpl.SetAttribute("FIELD_HELPER", "");
    tpl.SetAttribute("FIELD_NAME", "quantidade");
    tpl.SetAttribute("FIELD_VALUE", "");
    tpl.SetAttribute("FIELD_ICON", "<span class='glyphicon glyphicon-shopping-cart'></span>");

    return tpl.GetText();
}

