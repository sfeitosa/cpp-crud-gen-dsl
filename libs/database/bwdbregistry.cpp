#define __BWDBREGISTRY_CPP
#include "bwdbregistry.h"
#include <bwresultset.h>
#include <bwdb.h>

#include <iostream>

using namespace std;

#define DUMP(VAR) (cerr << #VAR << ": " << VAR << endl)
#define DUMPI(VAR,INDEX) (cerr << #VAR << "[" << #INDEX << "=" << INDEX << "]=" << VAR[INDEX] << endl)


BWDbRegistry::BWDbRegistry(const BWDb &db,
                           const BWString &dbtable,
                           BWList &fields) {
    bw_db = const_cast<BWDb*>(&db);
    reg_table = dbtable;
    SetTablePrefix("");
    SetIdField("");
    reg_fields = fields;
    m_keepsync = false;
    m_dbchanged = false;
    m_transactioncontrol = true;
}

BWDbRegistry::~BWDbRegistry() {
}

/*!
    \fn BWDbRegistry::Create()
 */
BWDbRegistry& BWDbRegistry::Create()
{
  bw_regnew = true;
  Clear();

  return *this;
}


/*!
    \fn BWDbRegistry::Delete()
 */
BWDbRegistry& BWDbRegistry::Delete()
{
    BWKeyPair delkpr, delrelkpr;
    BWKeyRelation::iterator itr; 
 
    if (bw_regnew) {
        throw BWError(DB_ERROR, "Erro ao deletar registro nao salvo");
    }

    try {
        if (m_transactioncontrol) {
            bw_db->BeginTransaction();
        }

        for (itr = m_relations.begin(); itr != m_relations.end(); itr++) {
            EmptyRelation(itr->first);
            itr->second.Save();
        }

        delkpr[reg_idfield_name] = reg_id;

        if (m_keepsync) {
            delRegistry(reg_tablename, reg_idfield_name, reg_id);
        }

        bw_db->Delete(reg_tablename, delkpr);

        if (m_transactioncontrol) {
            bw_db->Commit();
        }
    }
    catch (BWError &e) {
        e.TrackError(m_BWErrorPlace, "Erro ao deletar registro");
        try {
            if (m_transactioncontrol && bw_db->AtTransaction()) {
                bw_db->Rollback();
            }
            throw e;
        }
        catch (BWError &ee) {
            ee.TrackError(m_BWErrorPlace,
                          "Erro ao tratar problemas em remocao de registro");
            throw BWFatalError(DB_ERROR,
                               "Erro fatal ao aplicar tratamento de erros");
        }
    }

    return *this;
}

/*!
    \fn BWDbRegistry::Insert()
 */
BWDbRegistry& BWDbRegistry::Insert() {
    BWKeyRelation::iterator itr;

    if (m_keepsync) {
        SetField("ultimaalteracao", get_date(BWMASK_DTNOW));
    }

    try {
        if (m_transactioncontrol) {
            bw_db->BeginTransaction();
        }
        bw_db->Insert(reg_tablename, reg_new_values);

        SetId(bw_db->GetLastId(reg_tablename, reg_idfield_name));

        if (m_relations.size() > 0) {
            for (itr = m_relations.begin(); itr != m_relations.end(); itr++) {
                itr->second.Save();
            }
        }

        if (m_transactioncontrol) {
            bw_db->Commit();
        }
    }
    catch (BWError &e) {
        e.TrackError(m_BWErrorPlace,
                     "Erro ao inserir dados. Tentando Rollback...");

        if (m_transactioncontrol && bw_db->AtTransaction()) {
            bw_db->Rollback();
        }

        throw e;
    }
    return *this;
}


/*!
    \fn BWDbRegistry::Load(BWKeyPair &kpr)
 */
BWDbRegistry& BWDbRegistry::Load(BWKeyPair &kpr)
{
  BWString q("SELECT ");
  BWResultSet rs;
  unsigned int i;
  BWList fields,row;
  Clear();
  BWKeyRelation::iterator itr;

  q += reg_fields.ToString() + " FROM " + reg_tablename + " WHERE " + kpr.ToSQLWhere();

  try
  {
    rs = bw_db->ExecuteQuery(q);
    if (!rs.GetNumRows()) {
                throw BWError(DB_ERROR,
                              "Erro ao carregar: registro inexistente");
    }
    row = rs.GetRow(1);
    fields = rs.GetFieldList();

    for (i = 0; i < fields.size(); i++)
    {
      reg_curr_values[fields[i]] = row[i];
    }

    reg_new_values = reg_curr_values;

    for (itr = m_relations.begin(); itr != m_relations.end(); itr++) {
        itr->second.Load(reg_id);
    }

    bw_regnew = false;
  }
  catch (BWError &e)
  {
    e.TrackError(m_BWErrorPlace, 
                 BWString("Erro carregando dados: SQL=") + q);
    throw e;
  }

  return *this;
}


/*!
    \fn BWDbRegistry::Load(int id)
 */
BWDbRegistry& BWDbRegistry::Load(int id)
{
  BWKeyPair k;
  reg_id = "";
  reg_id.AppendInt(id);

  k[reg_idfield_name] = reg_id;
  Load(k);

  return *this;
}


/*!
    \fn BWDbRegistry::Save()
 */
BWDbRegistry& BWDbRegistry::Save()
{
    if (bw_regnew) {
        Insert();
    }
    else {
        Update();
    }

    return *this;
}


/*!
    \fn BWDbRegistry::SaveAs(int id)
 */
BWDbRegistry& BWDbRegistry::SaveAs(int id)
{
  SetId(id);
  return Save();
}


/*!
    \fn BWDbRegistry::SaveAsNew()
 */
BWDbRegistry& BWDbRegistry::SaveAsNew()
{
  SetId(0);

  return Save();
}


/*!
    \fn BWDbRegistry::Set(BWKeyPair &fields)
 */
BWDbRegistry& BWDbRegistry::Set(BWKeyPair &fields)
{
  BWKeyPair::iterator i = fields.begin();

  for (; i != fields.end(); i++)
  {
    SetField(i->first, i->second);
  }

  return *this;
}

/*!
    \fn BWDbRegistry::SetDb(const BWDb &db)
 */
BWDbRegistry& BWDbRegistry::SetDb(const BWDb &db) {
  bw_db = const_cast<BWDb*>(&db);
  m_dbchanged = true;
  return *this;
}


/*!
    \fn BWDbRegistry::SetField(const BWString &field, const BWString &value)
 */
BWDbRegistry& BWDbRegistry::SetField(const BWString &field, const BWString &v)
{
  unsigned int i;
  bool found = false;
  BWString value = v;

  value.Escape("\\");

  for (i = 0; i < reg_fields.size(); i++)
  {
    if (reg_fields[i] == field)
    {
      found = true;
      break;
    }
    else {
//             cerr << field << " nao eh " << reg_fields[i] << "!" << endl;
    }
  }

  if (found)
  {
    reg_new_values[field] = value;
  }
  else {
//               cerr << "Campo nao usado: " << field << "=" << value << endl;
  }

  return *this;
}


/*!
    \fn BWDbRegistry::SetRelation(const BWString &field, BWList &value)
 */
BWDbRegistry& BWDbRegistry::SetRelation(const BWString &field, BWList &value)
{

    if (m_relations.find(field) != m_relations.end()) {
        m_relations.find(field)->second.Add(value);
    }

    return *this;
}


/*!
    \fn BWDbRegistry::SetRelation(const BWString &field, const BWString &newvalue)
 */
BWDbRegistry& BWDbRegistry::SetRelation(const BWString &field, const BWString &newvalue)
{
    if (m_relations.find(field) != m_relations.end()) {
        m_relations.find(field)->second.AddValue(newvalue);  
    }

    return *this;
}


/*!
    \fn BWDbRegistry::Update()
 */
BWDbRegistry& BWDbRegistry::Update()
{
    BWKeyPair id;
    BWKeyRelation::iterator itr;
    BWKeyPair::iterator it;

    if (m_keepsync) {
        SetField("ultimaalteracao", get_date(BWMASK_DTNOW));
    }

    id[reg_idfield_name] = reg_id;

    try {
        if (m_transactioncontrol) {
            bw_db->BeginTransaction();
        }

        if (reg_new_values != reg_curr_values) {
            for (it = reg_new_values.begin(); 
                 it != reg_new_values.end(); 
                 it++) {
                if (it->second == "") {
                    reg_new_values.erase(it);
                }
            }

            if (!bw_db->Update(reg_tablename, reg_new_values, id)) {
                throw BWError(DB_ERROR, "Erro em Update() do banco");
            }
        }

        for (itr = m_relations.begin(); itr != m_relations.end(); itr++) {
            itr->second.Save();
        }

        if (m_transactioncontrol) {
            bw_db->Commit();
        }
    }
    catch (BWError &e) {
        e.TrackError(m_BWErrorPlace,
                     "Erro ao atualizar dados. Tentando Rollback...");
        try {
            if (m_transactioncontrol && bw_db->AtTransaction()) {
                bw_db->Rollback();
            }
            throw e;
        }
        catch (BWError &ee) {
            ee.TrackError(m_BWErrorPlace,
                    "Erro ao tratar problemas em atualizacao de registro");
            throw BWFatalError(DB_ERROR,
                         "Erro fatal no tratamento de erros");
        }
    }

    Load(reg_id.GetInt());

    return *this;
}


/*!
    \fn BWDbRegistry::GetTable() const
 */
BWString BWDbRegistry::GetTable() const
{
  return reg_tablename;
}


/*!
    \fn BWDbRegistry::GetField(const BWString &field) const
 */
BWString BWDbRegistry::GetField(const BWString &field)
{
  return reg_curr_values[field];
}


/*!
    \fn BWDbRegistry::operator[](const BWString &field) const
 */
BWString BWDbRegistry::operator[](const BWString &field)
{
  return GetField(field);
}


/*!
    \fn BWDbRegistry::GetId() const
 */
int BWDbRegistry::GetId() const
{
  return reg_id.GetInt();
}


/*!
    \fn BWDbRegistry::GetRelations(const BWString &rel)
 */
BWString BWDbRegistry::GetRelations(const BWString &rel)
{
    if (m_relations.find(rel) != m_relations.end()) {
        return m_relations.find(rel)->second.Get();
    }

    return "";
}


/*!
    \fn BWDbRegistry::AddField(const BWString &field)
 */
BWDbRegistry& BWDbRegistry::AddField(const BWString &field)
{
  reg_fields.push_back(field);
  return *this;
}


/*!
    \fn BWDbRegistry::AddRelation(const BWString &table)
 */
BWDbRegistry& BWDbRegistry::AddRelation(const BWString &name, BWDbRelation r) {
    m_relations[name] = r;
    
    return *this;
}


/*!
    \fn BWDbRegistry::SetIdField(const BWString &id)
 */
BWDbRegistry& BWDbRegistry::SetIdField(const BWString &id)
{
  unsigned int i;
  bool found = false;

  if (id == "") {
            reg_idfield_name = "id";
  }
  else {
              reg_idfield_name = id;
  }

  for (i = 0; i < reg_fields.size(); i++)
  {
    if (reg_fields[i] == reg_idfield_name)
    {
      found = true;
    }
  }
  if (!found)
  {
    reg_fields.push_back(reg_idfield_name);
  }

  return *this;
}


/*!
    \fn BWDbRegistry::SetTablePrefix(const BWString &p)
 */
BWDbRegistry& BWDbRegistry::SetTablePrefix(const BWString &p)
{
  reg_tabprefix = p;
  if (p == "")
  {
    reg_tablename = reg_table;
  }
  else
  {
    reg_tablename = p + "_" + reg_table;
  }

  return *this;
}

void BWDbRegistry::setRelationsId(int id) {
    BWKeyRelation::iterator itr;

    for (itr = m_relations.begin(); itr != m_relations.end(); itr++) {
        itr->second.SetId(id);
    }
}

/*!
    \fn BWDbRegistry::SetId(int id)
 */
BWDbRegistry& BWDbRegistry::SetId(int id) {
    reg_id = "";

    if (id > 0) {
        reg_id.AppendInt(id);
        setRelationsId(id);
    }
    else {
        bw_regnew = true;
    }

    return *this;
}


/*!
    \fn BWDbRegistry::Clear()
 */
BWDbRegistry& BWDbRegistry::Clear() {
    reg_curr_values.clear();
    reg_new_values.clear();  

    return *this;
}


/*!
    \fn BWDbRegistry::EmptyRelation(const BWString &relation)
 */
BWDbRegistry& BWDbRegistry::EmptyRelation(const BWString &relation)
{
    if (m_relations.find(relation) != m_relations.end()) {
        m_relations.find(relation)->second.Clear();
    }   
 
    return *this;
}


/*!
    \fn BWDbRegistry::GetRelationOptionIds(const BWString &relation)
 */
BWList BWDbRegistry::GetRelationOptionIds(const BWString &rel, const BWString &idfield) {
    BWList fields;

    try {
        if (m_relations.find(rel) != m_relations.end()) {
            fields.push_back(idfield);
            return m_relations.find(rel)->second.GetInfo(fields)[idfield];
        }
    }
    catch (BWError &e) {
        if (e.IsFatal()) {
            throw e;
        }
    }

    return fields;
}

/*!
    \fn BWDbRegistry::GetRelationOptionDescrs(const BWString &rel)
 */
BWList BWDbRegistry::GetRelationOptionDescrs(const BWString &rel, 
                     const BWString &descrfield) {
    BWList fields;

    try {
        if (m_relations.find(rel) != m_relations.end()) {
            fields.push_back(descrfield);
            return m_relations.find(rel)->second.GetInfo(fields)[descrfield];
        }
    }
    catch (BWError &e) {
        if (e.IsFatal()) {
            throw e;
        }
    }


    return fields;
}


/*!
    \fn BWDbRegistry::GetFieldOptionIds(const BWString &relation)
 */
BWList BWDbRegistry::GetFieldOptionIds(const BWString &relation, 
                                       const BWString &idfield) {
    return getInfo(relation, idfield);
}


/*!
    \fn BWDbRegistry::GetFieldOptionDescrs(const BWString &rel)
 */
BWList BWDbRegistry::GetFieldOptionDescrs(const BWString &rel, 
                                          const BWString &descrfield) {
    return getInfo(rel, descrfield);
}

BWList BWDbRegistry::getInfo(const BWString &table, const BWString &field) {
    BWResultSet rset;
    BWString query;
    BWString ntable;
    BWString prefix = "bw";

    if (reg_tabprefix != "") {
        prefix = reg_tabprefix;
    }

    if (table.find(prefix) == 0) {
        ntable = table;
    }
    else {
        ntable = prefix + "_" + table;
    }

    query  = "SELECT " + field + " FROM " + ntable;

    // Código adicionado para filtrar os registros removidos
    // na nova interface Ajax BeanWeb e manter a compatibilidade
    // da antiga versão
    if (!m_transactioncontrol) {
        query += " WHERE ativo = 'S'";
    }


    query += " ORDER BY id";

    rset = bw_db->ExecuteQuery(query);

    return rset.GetColumn(field); 
}

void BWDbRegistry::SetKeepSync() {
    m_keepsync = true;

    AddField("ultimaalteracao");
}

void BWDbRegistry::delRegistry(const BWString &table, const BWString &idfield,
                               const BWString &idvalue) {
    BWKeyPair record;

    record["tabela"] = table;
    record["ultimaalteracao"] = get_date(BWMASK_DTNOW);

    bw_db->Insert("bw_ksremovido", record);

    record.clear();
    record["ksremovido_id"] = BWString("").AppendInt(
                                bw_db->GetLastId("bw_ksremovido"));
    record["chave"] = idfield;
    record["valor"] = idvalue;

    bw_db->Insert("bw_ksremovidoassoc", record);
}

bool BWDbRegistry::DbChanged() {
    return m_dbchanged;
}

void BWDbRegistry::EnableTransactionControl() {
    m_transactioncontrol = true;
}

void BWDbRegistry::DisableTransactionControl() {
    m_transactioncontrol = false;
}

