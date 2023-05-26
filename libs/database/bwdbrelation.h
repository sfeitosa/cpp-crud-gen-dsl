//
// C++ Interface: bwdbrelation
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWDBRELATION_H
#define BWDBRELATION_H

#include <bwlist.h>
#include <bwkeylist.h>
#include <bwdb.h>
#include <bwutils.h>
#include <bwerror.h>
#include <bwlog.h>

/**
Classe responsavel por manipular relacoes em tabelas do banco de dados.

 @author Samuel da Silva Feitosa
*/
class BWDbRelation {
    public:
        BWDbRelation();
        BWDbRelation(BWDb *, const BWString &, const BWString &,
                     const BWString &invert = "false", 
                     const BWString &prefix = "bw");
        ~BWDbRelation();

        void SetFields(const BWString &, const BWString &);
        void SetRefTable(const BWString &);
        void SetKeepSync();
        void SetPrefix(const BWString &);
        void SetId(int id);
        void SetId(const BWString &id);
        void SetOrderField(const BWString &);

        void Save();

        void Add(BWList &);
        void AddValue(const BWString &);

        BWString Get();
        BWKeyList GetInfo(BWList &);

        void Load(const BWString &id);

        void Clear();
        
        void UseActiveField();

    private:
        void init();
        void emptyItens();
        void deleteItens(BWList &);
        void insertItens(BWList &);
        void emptyRelations(const BWString &, const BWString &);
        void delRelations(BWKeyPair data);

        BWList getDeleteList();
        BWList getInsertList();

        BWDb *m_db;
        BWString m_id;
        BWString m_from;
        BWString m_to;
        BWString m_prefix;
        bool m_invert;
        bool m_keepsync;
        BWString m_reftable; // tabela de relacionamentos
        BWString m_worktable; // tabela relacionada ao formulario
        BWString m_orderby;
        BWString m_field_from;
        BWString m_field_to;
        BWList m_cur_values;
        BWList m_new_values;
        BWLog m_log;
        bool m_activefield;
};

#endif // BWDBRELATION_H

