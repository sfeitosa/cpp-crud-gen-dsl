//
// C++ Implementation: bwlicensemanager
//
// Description: 
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwlicensemanager.h"

BWLicenseManager::BWLicenseManager() {
    m_log(BWLog::MSG_XDEBUG) << "Iniciando BWLicenseManager" << bwlog::endl;

    init();
}

BWLicenseManager::BWLicenseManager(const BWString &dbrc) {
    m_log(BWLog::MSG_XDEBUG) << "Iniciando BWLicenseManager" << bwlog::endl;

    init();

    m_db = bwdb_load(dbrc, true);
}

BWLicenseManager::~BWLicenseManager() {
}

bool BWLicenseManager::Register(const BWString &id, const BWString &code) {
    BWString url;
    BWString macaddr;
    BWString rsp;
    BWKeyPair response;
    //BWKeyPair macaddrs;
    BWKeyPair postdata;
    BWKeyPair data;
    BWUrlHandler conn;

    //macaddrs = getifacemacaddr();

    //macaddr = macaddrs[m_cfg["interface"]];
    macaddr = getmacaddr(m_cfg["interface"]);

    url  = m_cfg["proto"];
    url += "://";
    url += m_cfg["ip"];
    url += "/bw/license.cgi?op=register";

    m_log(BWLog::MSG_XDEBUG) << "Register::Conectando em: " << url
                             << bwlog::endl;

    conn.SetUrl(url);

    postdata["empresa_id"] = id;
    postdata["actcode"] = code;
    postdata["macaddr"] = macaddr;
    postdata["hash"] = generateHash(id, macaddr);

    m_log(BWLog::MSG_XDEBUG) << "Register::Enviando: " << postdata.ToString() 
                             << bwlog::endl;

    rsp = conn.Post(postdata);

    response.FromString(rsp);

    m_log(BWLog::MSG_XDEBUG) << "Register::Recebido: " << rsp << bwlog::endl;

    if (response["status"] == "ERR") {
        throw BWError(APP_ERROR, response["msg"]);
    }
 
    if (response["hash"] != generateHashResponse(id, macaddr)) {
        throw BWError(APP_ERROR, "Hash de identificacao invalido");
    }

    data["id"] = id;
    data["descr"] = response["descr"];

    m_db->Insert("bw_empresa", data);
   
    return true;
}

bool BWLicenseManager::Validate(const BWString &id) {
    BWString url;
    BWString macaddr;
    BWString rsp;
    BWKeyPair response;
    //BWKeyPair macaddrs;
    BWKeyPair postdata;
    BWUrlHandler conn;

    //macaddrs = getifacemacaddr();

    //macaddr = macaddrs[m_cfg["interface"]];
    macaddr = getmacaddr(m_cfg["interface"]);

    m_log(BWLog::MSG_XDEBUG) << "MACADDR: " << macaddr << bwlog::endl;

    url  = m_cfg["proto"];
    url += "://";
    url += m_cfg["ip"];
    url += "/bw/license.cgi?op=validate";

    m_log(BWLog::MSG_XDEBUG) << "Register::Conectando em: " << url
                             << bwlog::endl;

    conn.SetUrl(url);

    postdata["empresa_id"] = id;
    postdata["hash"] = generateHash(id, macaddr);

    m_log(BWLog::MSG_XDEBUG) << "Validate::Enviando: " << postdata.ToString()
                             << bwlog::endl;

    rsp = conn.Post(postdata);

    m_log(BWLog::MSG_XDEBUG) << "Validate::Recebido: " << rsp << bwlog::endl;

    response.FromString(rsp);

    if (response["status"] == "ERR") {
        throw BWError(APP_ERROR, response["msg"]);
    }

    if (response["hash"] != generateHashResponse(id, macaddr)) {
        throw BWError(APP_ERROR, "Hash de identificacao invalido");
    }

    WriteLicenseFile(id, macaddr, response["dataexpira"]);

    return true;
}

BWString BWLicenseManager::generateHash(const BWString &empresa_id,
                                        const BWString &macaddr) {
    BWString hash_str;
    BWString data = get_date("%Y-%m-%d");

    hash_str  = "BW_";
    hash_str += empresa_id;
    hash_str += "_";
    hash_str += macaddr;
    hash_str += "_";
    hash_str += data;
    hash_str += "_HASH_STR";

    return hash_str.Hash();
}

BWString BWLicenseManager::generateHashResponse(const BWString &empresa_id,
                                                const BWString &macaddr) {
    BWString hash_str;
    BWString data = get_date("%Y-%m-%d");

    hash_str  = "BW_";
    hash_str += empresa_id;
    hash_str += "_";
    hash_str += macaddr;
    hash_str += "_";
    hash_str += data;
    hash_str += "RESPONSE_HASH_STR";

    return hash_str.Hash();
}

BWString BWLicenseManager::RegisterClient(const BWString &hash, 
                           const BWString &id, const BWString &code,
                           const BWString &mac, const BWString &iporig) {
    BWString response;
    BWKeyPair record;
    BWKeyPair where;

    if (hash != generateHash(id, mac)) {
        response = "status='ERR' msg='Hash Invalido'";
        writeRegistrationLog(id, iporig, "ERR", "Hash Invalido");
        return response;
    }

    if (alreadyRegistered(id)) {
        response = "status='ERR' msg='Cliente ja registrado'";
        writeRegistrationLog(id, iporig, "ERR", "Cliente ja registrado");
        return response;
    }

    if (!validateCode(id, code)) {
        response = "status='ERR' msg='Codigo de ativacao invalido'";
        writeRegistrationLog(id, iporig, "ERR", "Codigo de ativacao invalido");
        return response;
    }
    
    where["id"] = id;
    where["codativacao"] = code;
    record["macaddr"] = mac;
    record["dataregistro"] = get_date(BWMASK_DTNOW);

    m_db->Update("bw_cliente", record, where);

    response  = "status='OK' hash='";
    response += generateHashResponse(id, mac);
    response += "' descr='";
    response += getClientDescr(id);
    response += "'"; 
    
    writeRegistrationLog(id, iporig, "OK", "");

    return response;
}

BWString BWLicenseManager::ValidateClient(const BWString &hash, 
                                          const BWString &id,
                                          const BWString &iporig) {
    BWString response;
    BWString mac = getClientMacAddr(id);
    BWString query;
    BWResultSet rset;

    if (hash != generateHash(id, mac)) {
        response = "status='ERR' msg='Hash Invalido'";
        writeValidationLog(id, iporig, "ERR", "Hash Invalido");
        return response;
    }

    if (!alreadyRegistered(id)) {
        response = "status='ERR' msg='Cliente nao esta registrado'";
        writeValidationLog(id, iporig, "ERR", "Cliente nao esta registrado");
        return response;
    }

    query  = "SELECT dataexpira FROM bw_cliente WHERE id = ";
    query += id + " AND macaddr = '" + mac + "'";

    rset = m_db->ExecuteQuery(query);

    if (rset.GetNumRows() <= 0) {
        response = "status='ERR' msg='MAC Address invalido'";
        writeValidationLog(id, iporig, "ERR", "MAC Address Invalido");
        return response;
    }

    response  = "status='OK' dataexpira='";
    response += rset.GetString("dataexpira");
    response += "' hash='";
    response += generateHashResponse(id, mac);
    response += "'";
    
    writeValidationLog(id, iporig, "OK", "");

    return response;
}

bool BWLicenseManager::alreadyRegistered(const BWString &id) {
    BWString query;
    BWResultSet rset;

    query  = "SELECT macaddr FROM bw_cliente WHERE id = ";
    query += id + " AND macaddr IS NOT NULL";

    rset = m_db->ExecuteQuery(query);

    if (rset.GetNumRows() > 0) {
        return true;
    }

    return false;
}

bool BWLicenseManager::validateCode(const BWString &id, const BWString &code) {
    BWString query;
    BWResultSet rset;

    query  = "SELECT codativacao FROM bw_cliente WHERE id = ";
    query += id + " AND codativacao = '" + code + "'";

    rset = m_db->ExecuteQuery(query);

    if (rset.GetNumRows() > 0) {
        return true;
    }

    return false;
}

BWString BWLicenseManager::getClientMacAddr(const BWString &id) {
    BWString query;
    BWResultSet rset;

    query = "SELECT macaddr FROM bw_cliente WHERE id = ";
    query += id;

    rset = m_db->ExecuteQuery(query);

    if (rset.GetNumRows() > 0) {
        return rset.GetString("macaddr");
    }

    return "";
}

BWString BWLicenseManager::generateHashFile(const BWString &id, 
                          const BWString &mac, const BWString &data, 
                          const BWString &dataexpira) {
    BWString str = "BW_";
    
    str += id + "_" + data + "_" + dataexpira + "_" + mac + "_LIC_FILE";

    return str.Hash();
}

void BWLicenseManager::WriteLicenseFile(const BWString &id, 
                      const BWString &mac, const BWString &dataexpira) {
    BWString str;
    BWString data = get_date("%Y-%m-%d");
    fstream file(BW_LIC_FILE, fstream::out);

    str  = id;
    str += "|";
    str += data;
    str += "|";
    str += dataexpira;
    str += "|";

    str += generateHashFile(id, mac, data, dataexpira);

    file << str << flush;
}

bool BWLicenseManager::ValidateLicenseFile() {
    fstream file(BW_LIC_FILE, fstream::in);
    BWString line;
    BWList tokens;

    getline(file, line);

    tokens = line.Split("|");

    if (!validateHashFile(tokens[0], tokens[1], tokens[2], tokens[3])) {
        return false;
    }

    if (!validateLicenseDate(tokens[1], tokens[2])) {
        return false;
    }

    return true;
}

bool BWLicenseManager::validateHashFile(const BWString &id, 
                      const BWString &datavalida, const BWString &dataexpira, 
                      const BWString &hash) {
    BWString macaddr;
    //BWKeyPair macaddrs;

    //macaddrs = getifacemacaddr();

    //macaddr = macaddrs[m_cfg["interface"]];
    macaddr = getmacaddr(m_cfg["interface"]);

    if (hash != generateHashFile(id, macaddr, datavalida, dataexpira)) {
        return false;
    }

    return true;
}

bool BWLicenseManager::validateLicenseDate(const BWString &datavalida, 
                                           const BWString &dataexpira) {
    BWResultSet rset;
    BWString query;

    query  = "SELECT 1::integer WHERE '";
    query += datavalida + "'::date > now()::date - 10";

    rset = m_db->ExecuteQuery(query);

    if (rset.GetNumRows() <= 0) {
        return false;
    }

    query  = "SELECT 1::integer WHERE '";
    query += dataexpira + "'::date >= now()::date";

    if (rset.GetNumRows() <= 0) {
        return false;
    }

    return true;
}

BWString BWLicenseManager::getClientDescr(const BWString &id) {
    BWString query;
    BWResultSet rset;

    query = "SELECT descr FROM bw_cliente WHERE id = " + id;

    rset = m_db->ExecuteQuery(query);

    if (rset.GetNumRows() > 0) {
        return rset.GetString("descr");
    }

    return "";
}

BWString BWLicenseManager::GetEmpresaId() {
    BWString query;
    BWResultSet rset;

    query = "SELECT id FROM bw_empresa";

    rset = m_db->ExecuteQuery(query);

    if (rset.GetNumRows() > 0) {
        return rset.GetString("id");
    }

    return "";
}

void BWLicenseManager::writeValidationLog(const BWString &emp, 
                       const BWString &ip, const BWString &status,
                       const BWString &msg) {
    BWKeyPair record;

    record["cliente_id"] = emp;
    record["iporigem"] = ip;
    record["status"] = status;
    record["msg"] = msg;
    record["datahora"] = get_date(BWMASK_DTNOW);

    m_db->Insert("bw_cliente_validacao", record);

}

void BWLicenseManager::writeRegistrationLog(const BWString &emp, 
                       const BWString &ip, const BWString &status,
                       const BWString &msg) {
    BWKeyPair record;

    record["cliente_id"] = emp;
    record["iporigem"] = ip;
    record["status"] = status;
    record["msg"] = msg;
    record["datahora"] = get_date(BWMASK_DTNOW);

    m_db->Insert("bw_cliente_registro", record);
}

void BWLicenseManager::init() {
    BWLog::Severity sev;

    m_cfg["proto"] = "http";
    m_cfg["interface"] = "eth0";
    m_cfg["ip"] = "200.135.239.125";

    m_cfg.Load(BW_LIC_CONF);

    if (m_cfg["loglevel"] != "" && m_cfg["loglevel"] != "(null)") {
        sev = static_cast<BWLog::Severity>(m_cfg["loglevel"].GetInt());
        m_log.SetLogLevel(sev);
    }

    m_log(BWLog::MSG_XDEBUG) << "CFG: " << m_cfg.ToString() << bwlog::endl;
}

