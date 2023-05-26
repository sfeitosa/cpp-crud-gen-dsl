// //
// C++ Implementation: bwhttp
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwhttp.h"

namespace Http
{

  BWHttp::BWHttp(char **envp)
      : BWHttpRequest(envp), BWHttpResponse()
  {
    BWKeyPair::iterator itr;
//             fstream logger("/tmp/log_web_cgi", fstream::out | fstream::app);

    itr = BWHttpRequest::cookies.begin();
    while (itr != BWHttpRequest::cookies.end())
    {
//                         logger << "Setando cookie " << (*itr).first << endl;
      if (itr->first != "" && itr->first != "(null)" && 
          itr->second != "" && itr->second != "(null)")
      {
            SetCookie(itr->first, itr->second);
      }
      itr++;
    }

    if (GetServerVar("REQUEST_URL") == "(null)" || GetServerVar("REQUEST_URL") == "") {
        if (GetServerVar("HTTPS") == "on") {
            env_vars["REQUEST_URL"] = "https://";
        }
        else {
            env_vars["REQUEST_URL"] = "http://";
        }
            env_vars["REQUEST_URL"] += GetServerVar("SERVER_NAME");

            if (GetServerVar("SERVER_PORT") != "80") {
                env_vars["REQUEST_URL"] += ":" + GetServerVar("SERVER_PORT");
            }

            env_vars["REQUEST_URL"] += GetServerVar("REQUEST_URI");
        }
  }


  BWHttp::~BWHttp()
{}
}


/*!
    \fn Http::BWHttp::SetCookie(const BWString &name, const BWString &valor)
 */
void Http::BWHttp::SetCookie(const BWString &name, const BWString &valor)
{
  BWHttpResponse::SetCookie(name, valor);
  BWHttpRequest::cookies[name] = valor;
}
