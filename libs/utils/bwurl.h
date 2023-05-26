#ifndef BWURL_H
#define BWURL_H

#include "utils.h"

/**
Classe responsavel por parsing e geracao de URLs
Formato de URL:
protocolo://[usuario[:senha]@]host[:porta][/caminho[?query-string]]
*/
class BWUrl
{
  public:
    BWUrl();
    BWUrl(const BWString &url);
    BWUrl(BWUrl &url);
    ~BWUrl();
  private:
    BWString m_url;
    BWString m_host;
    BWString m_pass;
    BWString m_path;
    BWString m_port;
    BWString m_user;
    BWString m_protocol;
    BWString m_querystring;
    BWKeyPair m_parmlist;
  public:
    BWString GetHost() const;
    BWString GetPass() const;
    BWString GetRequestPath() const;
    int GetPort() const;
    BWString GetUser() const;
    BWString GetProtocol() const;
    BWString GetDomainSQLList();
    BWList GetDomainList();
    BWUrl& operator= (const BWString &s);
    BWString GetFile() const;
    BWString GetPath() const;
    BWString GetQueryString() const;
    BWString GetParameter(const BWString &param);
    void SetUrl(const BWString &url);
    BWString GetUrl();
  protected:
    void parseUrl();
    void setUrl(const BWString &url);
    void composeUrl();
};

#endif
