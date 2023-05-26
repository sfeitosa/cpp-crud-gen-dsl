//
// C++ Implementation: bwurl
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwurl.h"

BWUrl::BWUrl() {}

BWUrl::BWUrl(const BWString &url)
{
  setUrl(url);
}

BWUrl::BWUrl(BWUrl &url)
{/*
                            m_host=url.GetHost();
                            m_pass=url.GetPass();
                            m_path=url.GetPath();
                            m_port=url.GetPort();
                            m_protocol=url.GetProtocol();
                            m_user=url.GetUser();
                   */
  setUrl(url.GetUrl());
}

void BWUrl::composeUrl()
{
  m_url = m_protocol;
  m_url += "://";
  if (m_user != "")
  {
    m_url += m_user;
    if (m_pass != "")
    {
      m_url += ":";
      m_url += m_pass;
    }
    m_url += "@";
  }

  m_url += m_host;
  if (m_port != "")
  {
    m_url += ":";
    m_url += m_port;
  }

  m_url += "/";
  m_url += m_path;
}


BWUrl::~BWUrl()
{}

BWString BWUrl::GetHost() const
{
  return this->m_host;
}

int BWUrl::GetPort() const
{
  return this->m_port.GetInt();
}

BWString BWUrl::GetPass() const
{
  return this->m_pass;
}

BWString BWUrl::GetRequestPath() const
{
  return this->m_path;
}

BWString BWUrl::GetUser() const
{
  return this->m_user;
}

BWString BWUrl::GetProtocol() const
{
  return this->m_protocol;
}

void BWUrl::setUrl(const BWString &url)
{
  m_url = url;
  parseUrl();
}

void BWUrl::parseUrl()
{
  BWList tokens = m_url.Split("://", 2);
  BWList subtokens;
  BWList newtokens;
  BWString resto;
  BWString begin;
  BWString ends;

  if (tokens.size() == 1)
  {
    throw BWError(BASE_ERROR, "URL Invalida: Sem protocolo especificado");
  }
  m_protocol = tokens[0];

  tokens = tokens[1].Split("/", 2);
  
  begin = tokens[0];
  
  ends = "/";
  if (tokens.size() != 1)
  {
    ends += tokens[1];
  }

  subtokens = begin.Split("@", 2);

  if (subtokens.size() > 1) {
    newtokens = subtokens[0].Split(":", 2);
    m_user = newtokens[0];
    if (newtokens.size() > 1) {
      m_pass = newtokens[1];
    }
    resto = subtokens[1];
  }
  else {
    resto = begin;
  }

  tokens = resto.Split(":", 2);
  
  m_host = tokens[0];
  
  if (tokens.size() > 1)
  {
    m_port = tokens[1];
  }
  else
  {
    m_port = "80";
  }
  
  tokens = ends.Split("?", 2);
  
  m_path = tokens[0];
  if (tokens.size() > 1) {
              m_querystring = tokens[1];
  }
  
  m_parmlist = getqs(m_querystring);
}


/*!
    @fn BWUrl::GetDomainSQLList()
    @return Retorna lista de dominios do host na hierarquia de DNS, em formato a
    @return ser utilizado em um IN num banco SQL
 */
BWString BWUrl::GetDomainSQLList()
{
  BWList tokens = GetDomainList();
  BWString list("(");
  unsigned int tksz = tokens.size();

  for (unsigned int i = 0; i < tksz; i++)
  {
    list += '\'';
    list += tokens[i];
    list += '\'';

    if (i < tksz - 1)
    {
      list += ", ";
    }
  }

  list += ')';

  return list;
}


/*!
    @fn BWUrl::GetDomainList()
    @return Retorna uma lista de dominios do host, ordenados na hierarquia de DNS
 */
BWList BWUrl::GetDomainList()
{
  BWList tokens = m_host.Split(".");
  BWList ret;
  unsigned int size, i;
  BWString token, temptoken;

  size = tokens.size();
  i = size - 1;
  token = tokens[i];
  ret.insert(ret.begin(), token);

  while (i--)
  {
    temptoken = tokens[i] + "." + token;
    token = temptoken;
    ret.insert(ret.begin(), token);
  }

  return ret;
}



/*!
    \fn BWUrl::operator=(const BWString &s)
 */
BWUrl& BWUrl::operator= (const BWString &s)
{
  setUrl(s);
  return this[0];
}


/*!
    \fn BWUrl::GetFile() const
 */
BWString BWUrl::GetFile() const
{
  return BWString();
}


/*!
    \fn BWUrl::GetPath() const
 */
BWString BWUrl::GetPath() const
{
  return m_path;
}


/*!
    \fn BWUrl::GetQueryString() const
 */
BWString BWUrl::GetQueryString() const
{
  return m_querystring;
}


/*!
    \fn BWUrl::GetParameter(BWString &param)
 */
BWString BWUrl::GetParameter(const BWString &param)
{
  return m_parmlist.GetString(param);
}

/*!
    \fn BWUrl::SetUrl(BWString &url)
 */
void BWUrl::SetUrl(const BWString &url)
{
  setUrl(url);
}

/*!
    \fn BWUrl::GetUrl()
 */
BWString BWUrl::GetUrl()
{
  return m_url;
}
