#ifndef BWDBREGISTRY_H
#define BWDBREGISTRY_H

#include <bwlist.h>
#include <bwdb.h>
#include <bwlog.h>
#include <bwutils.h>
#include <bwdbrelation.h>
#include <map>

typedef std::map<BWString, BWDbRelation> BWKeyRelation;

#ifdef __BWDBREGISTRY_CPP

#ifndef __EMPTY_L
#define __EMPTY_L
BWList __dbreg_empty_list;
#endif//__EMPTY_L

#else
extern BWList __dbreg_empty_list;
#endif//__BWDBREGISTRY_CPP


class BWDbRegistry
{
  public:
   BWDbRegistry(const BWDb &db,
                const BWString &dbtable,
                BWList &fields=__dbreg_empty_list);

    ~BWDbRegistry();
    BWDbRegistry& Create();
    BWDbRegistry& Delete();
    BWDbRegistry& Insert();
    BWDbRegistry& Load(BWKeyPair &kpr);
    BWDbRegistry& Load(int id);
    BWDbRegistry& Save();
    BWDbRegistry& SaveAs(int id);
    BWDbRegistry& SaveAsNew();
    BWDbRegistry& Set(BWKeyPair &fields);
    BWDbRegistry& SetDb(const BWDb &db);
    BWDbRegistry& SetField(const BWString &field, const BWString &v);
    BWDbRegistry& SetRelation(const BWString &field, BWList &value);
    BWDbRegistry& SetRelation(const BWString &field, const BWString &newvalue);
    BWDbRegistry& Update();
    BWString GetTable() const;
    BWString GetField(const BWString &field);
    BWString operator[](const BWString &field);
    int GetId() const;
    BWString GetRelations(const BWString &rel);
    BWDbRegistry& AddField(const BWString &field);
    BWDbRegistry& AddRelation(const BWString &name, BWDbRelation rel);
    BWDbRegistry& SetIdField(const BWString &id);
    BWDbRegistry& SetTablePrefix(const BWString &p);
    BWDbRegistry& SetId(int id);
    BWDbRegistry& Clear();
    BWDbRegistry& EmptyRelation(const BWString &relation);
    BWKeyPair GetRelationOptions(const BWString &rel, const BWString &idfield="id", const BWString &descrfield="descr", const BWString &reltab="");
    BWList GetRelationOptionIds(const BWString &relation, const BWString &idfield="id");
    BWList GetRelationOptionDescrs(const BWString &rel, const BWString &descrfield="descr");

    BWList GetFieldOptionIds(const BWString &field, 
                             const BWString &idfield = "id");
    BWList GetFieldOptionDescrs(const BWString &field, 
                                const BWString &descrfield = "descr");

    void SetKeepSync();
    bool DbChanged();
    void EnableTransactionControl();
    void DisableTransactionControl();

  protected:
    void setRelationsId(int);
    void delRegistry(const BWString &table, const BWString &idfield, 
                     const BWString &idvalue);
    BWList getInfo(const BWString &, const BWString &);

    BWKeyRelation m_relations;
    BWKeyPair reg_curr_values;
    BWKeyPair reg_new_values;
    BWString reg_id;
    BWDb* bw_db;
    BWString reg_table;
    BWList reg_fields;
    BWString reg_idfield_name;
    BWList reg_relations;
    BWString reg_tabprefix;
    bool bw_regnew;
    BWString reg_tablename;
    bool m_keepsync;
    bool m_dbchanged;
    BWLog m_log;
    bool m_transactioncontrol;
};

#endif
