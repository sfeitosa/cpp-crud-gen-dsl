//
// C++ Interface: bwajaxdbhtmlform
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <bruno@beanweb.com.br>, (C) 2013
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CGIBWAJAXDBHTMLFORM_H
#define CGIBWAJAXDBHTMLFORM_H

#include <bwajaxhtmlform.h>
#include <bwdatabase.h>
#include <bwresultset.h>
#include <bwerror.h>
#include <bwlog.h>
#include <bwdbregistry.h>

namespace CGI
{

  /**
  Classe responsavel por intermediar banco de dados e formularios HTML

   @author Samuel da Silva Feitosa
  */
  class BWAjaxDbHtmlForm : public CGI::BWAjaxHtmlForm
  {
    public:
      BWAjaxDbHtmlForm(BWKeyPair &, const BWString &file, BWDb &db);
      BWAjaxDbHtmlForm(BWKeyPair &, BWConfig &cfg, BWDb &db);

      ~BWAjaxDbHtmlForm();
      BWString GetFieldText(const BWString &field);
      BWString Save();

    protected:
      BWDbRegistry *m_dbreg;
      BWKeyPair m_filter;
      BWString m_dbtable;
      BWString m_dbtabprefix;
      BWDb* m_db;
      BWKeyPair m_data;
      
    private:
      void fillRegistry();
      BWDbRelation getRelation(const BWString &);
      void setDataToSave();
      void init(BWDb &db);

      BWLog m_log;
  };

}

#endif
