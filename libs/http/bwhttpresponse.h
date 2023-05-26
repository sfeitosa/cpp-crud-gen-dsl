#ifndef BWHTTPBWHTTPRESPONSE_H
#define BWHTTPBWHTTPRESPONSE_H

#include <bwkeypair.h>
#include <bwlist.h>
#include <utils.h>
#include <bwstring.h>
#include <bwerror.h>
#include <iostream>

using namespace std;

namespace Http
{

  /**
  Classe de manipulacao de respostas HTTP

   @author
  */
  class BWHttpResponse
  {
    public:
      BWHttpResponse();
      ~BWHttpResponse();
      void Clear();
      void Reset();
      void SendResponse();
      void SetCookie(const BWString &name, const BWString &valor="");
      void SetHeader(const BWString &header, const BWString &valor);
      BWString GetHeader(const BWString &header);
      BWHttpResponse& operator<< (const BWString &data);
    void AppendFile(const BWString &fname);

    protected:
      BWKeyPair headers;
      BWList headerl;
      BWString m_data;
      bool m_data_sent;
      BWKeyPair cookies;
    protected:
      void append(const BWString &str);
  };

}

#endif
