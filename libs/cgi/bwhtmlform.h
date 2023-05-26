//
// C++ Interface: bwhtmlform
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CGIBWHTMLFORM_H
#define CGIBWHTMLFORM_H

#include <bwhttp.h>
#include <bwkeypair.h>
#include <bwlist.h>
#include <bwtemplate.h>
#include <bwconfig.h>
#include <fstream>

#define _BW_HTML_FIELDTYPES  "etc/fieldtypes/"

using namespace std;

namespace CGI
{
  using namespace Http;

  /**
  Classe responsavel por gerenciar formularios genericos, altamente configuraveis e com campos baseados em templates

   @author Bruno Moreira Guedes
  */
  class BWHtmlForm
  {
    public:
      BWHtmlForm(const BWString &file, BWHttp *channel);
      BWHtmlForm(BWConfig &cfg, BWHttp *channel);

      virtual ~BWHtmlForm();
      BWKeyPair GetFieldValues();
      BWString operator[](const BWString &idx);
      bool IsFilled();
      void AddField(const BWString &name, const BWString &type, const BWString &label, const BWString &defaultv);
      void Send();
      void SetChannel(Http::BWHttp* channel);
      void SetConfig(const BWString &parm, const BWString &val);
      void SetFieldParm(BWString &field, const BWString &parm, const BWString &value);

    protected:
      BWConfig bw_config;
      BWList bw_fields;
      BWString bw_formname;
      Http::BWHttp* bw_channel;
      BWString bw_configfile;
      BWString bw_btn_ok;
      BWString bw_btn_cancel;
    public:
      virtual BWString GetFieldText(const BWString &field)
      {
            return htmlFormGetFieldText(field);
      }

private:
    BWString htmlFormGetFieldText(const BWString &field);
    void init(BWConfig &cfg, BWHttp *channel);
  };

}

#endif
