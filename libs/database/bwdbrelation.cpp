#include "bwdbrelation.h"

BWDbRelation::BWDbRelation() {
    m_db = NULL;
    m_prefix = "bw";
    m_invert = false;
    m_keepsync = false;
    m_activefield = false;
}

BWDbRelation::BWDbRelation(BWDb *db, 
                           const BWString &to, const BWString &from,
                           const BWString &invert, const BWString &prefix) {
    m_db = db;
    m_from = from;
    m_to = to;
    m_prefix = prefix;
    m_invert = invert == "true";
    m_keepsync = false;
    m_activefield = false;

    init();
}


void BWDbRelation::init() {
    if (!m_invert) {
        m_reftable = m_prefix + "_" + m_to + "_" + m_from;
    }
    else {
        m_reftable = m_prefix + "_" + m_from + "_" + m_to;
    }

    m_worktable = m_prefix + "_" + m_from;
    SetFields(m_to + "_id", m_from + "_id");
}

BWDbRelation::~BWDbRelation() {
}

void BWDbRelation::SetFields(const BWString &to, const BWString &from) {
    m_field_from = from;
    m_field_to = to;
}

void BWDbRelation::SetRefTable(const BWString &t) {
    m_reftable = t;
}

void BWDbRelation::SetKeepSync() {
    m_keepsync = true;
}

void BWDbRelation::SetPrefix(const BWString &prefix) {
    m_prefix = prefix;

    init();
}

void BWDbRelation::SetId(int id) {
    m_id = "";

    m_id.AppendInt(id);
}

void BWDbRelation::SetId(const BWString &id) {
    m_id = id;
}

void BWDbRelation::SetOrderField(const BWString &field) {
    m_orderby = field;
}

void BWDbRelation::Save() {
    BWList ins;
    BWList del;

    if (m_new_values.size() <= 0) {
        // Nothing to do
        return;
    }

    if (!m_db->AtTransaction()) {
        throw BWError(DB_ERROR, "Erro: Salvando relacoes fora da transacao");
    }

    // Este teste indica que a lista de itens foi zerada, e portanto
    // devemos deletar todos os valores correspondentes a este ID
    if (m_new_values.size() == 1 &&
        m_new_values[0] == "<%!EMPTY!%>") {

        emptyItens();

        return;
    }

    del = getDeleteList();
    ins = getInsertList();

    deleteItens(del);
    insertItens(ins);
}

void BWDbRelation::emptyItens() {
    BWString where;

    where  = m_field_to + " = '" + m_id + "'";
    where += " AND " + m_field_from;
    where += " IS NOT NULL";

    if (m_keepsync) {
        // Informar ao keepsync que esta sendo removido
        emptyRelations(m_field_to, m_id);
    }

    m_db->Delete(m_reftable, where);
}

void BWDbRelation::deleteItens(BWList &del) {
    BWKeyPair values;

    values[m_field_to] = m_id;

    for (unsigned int i = 0; i < del.size(); i++) {
        values[m_field_from] = del[i];

        if (m_keepsync) {
            // Informar ao keepsync que esta sendo removido
            delRelations(values);
        }

        if (!m_db->Delete(m_reftable, values)) {
        	throw BWError(DB_ERROR, "Erro ao excluir dados da relação.");
        }
    }
}

void BWDbRelation::insertItens(BWList &ins) {
    BWKeyPair values;

    values[m_field_to] = m_id;

    for (unsigned int i = 0; i < ins.size(); i++) {
        values[m_field_from] = ins[i];

        if (m_keepsync) {
            values["ultimaalteracao"] = get_date(BWMASK_DTNOW);
        }

        if (!m_db->Insert(m_reftable, values)) {
        	throw BWError(DB_ERROR, "Erro ao salvar dados da relação.");
        }
    }       
}

BWList BWDbRelation::getDeleteList() {
    BWList list;
    bool still;

    for (unsigned int i = 0; i < m_cur_values.size(); i++) {
        still = false;
        for (unsigned int j = 0; j < m_new_values.size(); j++) {
            if (m_cur_values[i] == m_new_values[j]) {
                still = true;
            }
        }
        if (!still) {
            list.push_back(m_cur_values[i]);
        }
    }

    return list;
}

BWList BWDbRelation::getInsertList() {
    BWList list;
    bool still;
  
    for (unsigned int i = 0; i < m_new_values.size(); i++) {
        still = false;
        for (unsigned int j = 0; j < m_cur_values.size(); j++) {
            if (m_new_values[i] == m_cur_values[j]) {
                still = true;
            }
        }
        if (!still) {
            list.push_back(m_new_values[i]);
        }
    }

    return list;
}

void BWDbRelation::Add(BWList &value) {
    unsigned int i;

    for (i = 0; i < value.size(); i++) {
        AddValue(value[i]);
    }
}


void BWDbRelation::AddValue(const BWString &value) {
    m_new_values.push_back(value);
}


BWString BWDbRelation::Get() {
    return m_cur_values.ToString();
}

BWKeyList BWDbRelation::GetInfo(BWList &fields) {
    BWKeyList klist;
    BWResultSet rset;
    BWString query;

    if (m_orderby == "") {
        m_orderby = "id";
    }

    query  = "SELECT ";
    query += fields.ToString();
    query += "," + m_orderby + " AS bw_sort_field";
    query += " FROM " + m_worktable;

    if (m_activefield) {
        query += " WHERE ativo = 'S'";
    }

    query += " ORDER BY bw_sort_field";


    m_log(BWLog::MSG_XDEBUG) << "Query: " << query << bwlog::endl;

    rset = m_db->ExecuteQuery(query);

    for (unsigned int i = 0; i < fields.size(); i++) {
        klist[fields[i]] = rset.GetColumn(fields[i]);
    }

    return klist;
}


void BWDbRelation::Load(const BWString &id) {
    BWString query;
    BWResultSet rset;

    SetId(id);

    query  = "SELECT ";
    query += m_field_from;
    query += " FROM " + m_reftable;
    query += " WHERE " + m_field_to;
    query += " = '" + m_id + "'";
    query += " AND " + m_field_from;
    query += " IS NOT NULL";

    if (m_activefield) {
        query += " AND ativo = 'S'";
    }

    rset = m_db->ExecuteQuery(query);

    m_cur_values = rset.GetColumn(m_field_from);
}

// Tambem esta levando em consideracao somente a direita...verificar depois
void BWDbRelation::Clear()
{
    m_cur_values.clear();
    m_new_values.clear();
    m_new_values.push_back("<%!EMPTY!%>");
}


void BWDbRelation::emptyRelations(const BWString &idfield, 
                                  const BWString &idvalue) {
    BWKeyPair record;

    record["tabela"] = m_reftable;
    record["ultimaalteracao"] = get_date(BWMASK_DTNOW);

    m_db->Insert("bw_ksremovido", record);

    record.clear();

    record["ksremovido_id"] = BWString("").AppendInt(
                                           m_db->GetLastId("bw_ksremovido"));
    record["chave"] = idfield;
    record["valor"] = idvalue;

    m_db->Insert("bw_ksremovidoassoc", record);
}

void BWDbRelation::delRelations(BWKeyPair data) {
    BWKeyPair record;
    BWKeyPair::iterator itr;
    BWString id;

    record["tabela"] = m_reftable;
    record["ultimaalteracao"] = get_date(BWMASK_DTNOW);

    m_db->Insert("bw_ksremovido", record);

    id.AppendInt(m_db->GetLastId("bw_ksremovido"));

    for (itr = data.begin(); itr != data.end(); itr++) {
        record.clear();
        record["ksremovido_id"] = id;
        record["chave"] = itr->first;
        record["valor"] = itr->second;

        m_db->Insert("bw_ksremovidoassoc", record);
    }
}

void BWDbRelation::UseActiveField() {
    m_activefield = true;
}
