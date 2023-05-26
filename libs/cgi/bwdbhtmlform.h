//
// C++ Interface: bwdbhtmlform
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CGIBWDBHTMLFORM_H
#define CGIBWDBHTMLFORM_H

#include <bwhtmlform.h>
#include <bwdatabase.h>
#include <bwresultset.h>
#include <bwerror.h>
#include <bwlog.h>
#include <bwdbregistry.h>

namespace CGI
{

  /**
  Classe responsavel por intermediar banco de dados e formularios HTML

   @author Bruno Moreira Guedes
  */
  class BWDbHtmlForm : public CGI::BWHtmlForm
  {
    public:
      BWDbHtmlForm(const BWString &file, BWHttp *channel, BWDb &db);
      BWDbHtmlForm(BWConfig &cfg, BWHttp *channel, BWDb &db);

      ~BWDbHtmlForm();
      BWString GetFieldText(const BWString &field);
      void Save();

    protected:
      BWDbRegistry *m_dbreg;
      BWKeyPair bw_filter;
      BWString bw_dbtable;
      BWString bw_dbtabprefix;
      BWDb* bw_db;
      
    private:
      void fillRegistry();
      BWDbRelation getRelation(const BWString &);
      void setDataToSave();
      void init(BWDb &db);

      BWLog m_log;
  };

}

#endif
