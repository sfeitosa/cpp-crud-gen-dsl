//
// C++ Implementation: bwhttprequest
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwhttprequest.h"

namespace Http
{

  BWHttpRequest::BWHttpRequest(char **envp)
  {
    int temp_len, bytes, pos = 0;
    char *temp_chr = NULL;
    BWList cookl, par;
    unsigned int i;

//             fstream logger("/tmp/log_web_cgi", fstream::out | fstream::app);

    if (!envp)
    {
      throw BWError(HTTP_ERROR, "Ponteiro de variaveis de ambiente esta nulo");
    }

//             while(*envp){
//                         logger << *envp << endl;
//                         envp++;
//             }

    env_vars = envget(envp);
//              logger << "QUERY_STRING: " << env_vars["QUERY_STRING"] << endl;
//              logger << "CONTENT_LENGTH: " << env_vars["CONTENT_LENGTH"].GetInt() << endl;

    get_vars = getqs(env_vars["QUERY_STRING"]);

    input_len = env_vars["CONTENT_LENGTH"].GetInt();

    temp_len = input_len;

    temp_chr = new char[input_len + 1];
    temp_chr[input_len] = 0;

//             logger << "AQUI TAMBEM" << endl;

    while (temp_len)
    {

      bytes = read(0, temp_chr + pos, input_len);

      if (bytes < 0)
      {
        throw BWError(HTTP_ERROR, strerror(errno));
      }

      if (bytes == 0)
      {
        throw BWError(HTTP_ERROR, "Leitura terminada antes do esperado");
      }

      temp_len -= bytes;
      pos += bytes;
    }

    input_data = temp_chr;

    m_log(BWLog::MSG_XDEBUG) << "Input Data: " << input_data << bwlog::endl;

    post_vars = getqs(input_data);

    cookl = env_vars["HTTP_COOKIE"].Split("; ");
    for (i = 0; i < cookl.size(); i++)
    {
//                         logger << "Cookie #" << i << cookl[i] << endl;
      par = cookl[i].Split("=",2);
      if (par.size() == 2)
      {
        cookies[par[0]] = urldecode(par[1]);
      }
      else
      {
        cookies[par[0]] = "";
      }
    }

    delete[] temp_chr;
  }


  BWHttpRequest::~BWHttpRequest()
  {}

  BWString BWHttpRequest::GetServerVar(const BWString &var)
  {
    return env_vars[var];
  }

  BWString BWHttpRequest::GetParameter(const BWString &var)
  {
    BWString ret;

    if (GetPOSTParameter(var) == "(null)" && GetGETParameter(var) == "(null)")
    {
      return "(null)";
    }

    ret = GetPOSTParameter(var);
    if (ret == "(null)")
    {
      ret = GetGETParameter(var);
    }

    return ret;
  }

  BWString BWHttpRequest::GetPOSTParameter(const BWString &var)
  {
    return post_vars[var];
  }

  BWString BWHttpRequest::GetGETParameter(const BWString &var)
  {
    return get_vars[var];
  }


  BWString BWHttpRequest::GetFilePath(const BWString &file)
  {
    return file_paths[file];
  }

  /*!
      \fn BWHttp::BWHttpRequest::GetCookie(const BWString &cookie)
   */
  BWString BWHttpRequest::GetCookie(const BWString &cookie)
  {
    return cookies[cookie];
  }

}



/*!
    \fn Http::BWHttpRequest::UnsetParameter(const BWString &parm)
 */
void Http::BWHttpRequest::UnsetParameter(const BWString &parm)
{
  post_vars[parm] = "";
  get_vars[parm] = "";
}


/*!
    \fn Http::BWHttpRequest::GetServerVars()
 */
BWKeyPair& Http::BWHttpRequest::GetServerVars()
{
    return env_vars;
}


/*!
    \fn Http::BWHttpRequest::GetGetVars()
 */
BWKeyPair& Http::BWHttpRequest::GetGetVars()
{
    return get_vars;
}

/*!
    \fn Http::BWHttpRequest::GetCookieVars()
 */
BWKeyPair& Http::BWHttpRequest::GetCookieVars()
{
    return Http::BWHttpRequest::cookies;
}


/*!
    \fn Http::BWHttpRequest::GetPostVars()
 */
BWKeyPair& Http::BWHttpRequest::GetPostVars()
{
    return post_vars;
}


/*!
    \fn Http::BWHttpRequest::GetRequestVars()
 */
BWKeyPair Http::BWHttpRequest::GetRequestVars()
{
    BWKeyPair ret;
    ret.AppendKeyPair(GetGetVars());
    ret.AppendKeyPair(GetPostVars());
    ret.AppendKeyPair(GetCookieVars());
    ret.AppendKeyPair(GetServerVars());
    return ret;
}

BWString Http::BWHttpRequest::GetRawData() {
    return input_data;
}
