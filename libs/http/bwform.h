#ifndef BWHTTPBWFORM_H
#define BWHTTPBWFORM_H

#include <bwdbsqlite.h>
#include <bwkeypair.h>
#include <bwlist.h>
#include <bwkeylist.h>
#include <bwhttprequest.h>
#include <bwstring.h>
#include <bwerror.h>
#include <utils.h>
#include <fstream>
#include <bwhttp.h>

using namespace std;

namespace Http
{

  class BWForm
  {
    public:
      BWForm(const BWString &nomep, BWHttp *httpp);
      ~BWForm();
      BWString operator[](const BWString &field);
      BWString GetRawData(const BWString &field);
      bool IsFilled();
      void Ask();
      void Report();
      void Show();
      void Clear();
      void Load(const BWString &fname="");
      void AddField(const BWString &field, const BWString &tipo="text", const BWString defvalue="");
      void SetLabel(const BWString &fld, const BWString &labelp);
      void SetLabel(const BWString &l);

    protected:
      BWKeyPair fields;
      BWList fieldnames;
      BWKeyPair curfieldvalues;
      BWString nome;
      BWHttp *http;
      BWKeyPair labels;
      bool isLoaded;
      BWKeyList options;
      BWString fnamepre;
      BWString label;
    protected:
      void loadHttpData();
  };

}

#endif
