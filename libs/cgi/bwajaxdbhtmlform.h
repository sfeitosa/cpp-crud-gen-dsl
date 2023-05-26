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
