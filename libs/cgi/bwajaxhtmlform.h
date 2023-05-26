#ifndef CGIBWAJAXHTMLFORM_H
#define CGIBWAJAXHTMLFORM_H

#include <bwhttp.h>
#include <bwkeypair.h>
#include <bwlist.h>
#include <bwtemplate.h>
#include <bwconfig.h>
#include <fstream>

#define _BW_HTML_MODAL "var/www/html/templates/modal.html"
#define _BW_HTML_FORM "var/www/html/templates/form.html"
#define _BW_HTML_FIELDTYPES  "etc/fieldtypes/"

using namespace std;

namespace CGI
{
  using namespace Http;

  class BWAjaxHtmlForm
  {
    public:
      BWAjaxHtmlForm(BWKeyPair &, const BWString &file);
      BWAjaxHtmlForm(BWKeyPair &, BWConfig &cfg);

      virtual ~BWAjaxHtmlForm();
      BWKeyPair GetFieldValues();
      BWString operator[](const BWString &idx);
      bool IsFilled();
      bool WasSubmitted();
      void AddField(const BWString &name, const BWString &type, const BWString &label, const BWString &defaultv);
      BWString GetInnerHtml();
      //void SetConfig(const BWString &parm, const BWString &val);
      //void SetFieldParm(BWString &field, const BWString &parm, const BWString &value);

      virtual BWString GetFieldText(const BWString &field)
      {
            return htmlFormGetFieldText(field);
      }

    protected:
      BWConfig m_config;
      BWKeyPair m_data;
      BWList m_fields;
      BWString m_cad;
      BWLog m_log;

private:
    BWString htmlFormGetFieldText(const BWString &field);
    void init(BWConfig &cfg);
  };

}

#endif
