//
// C++ Interface: bwhttprequest
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWHTTPBWHTTPREQUEST_H
#define BWHTTPBWHTTPREQUEST_H
#include <bwstring.h>
#include <bwkeypair.h>
#include <bwlist.h>
#include <fstream>
#include <utils.h>
#include <bwerror.h>
#include <bwlog.h>
#include <errno.h>

#include <iostream>
#include <fstream>
#include <unistd.h>
using namespace std;

namespace Http
{

/**
  Requisicao HTTP

   @author
 */
class BWHttpRequest
{
    protected:
        int input_len;
        BWString input_data;
        BWString query_string;
        BWKeyPair env_vars;
        BWKeyPair post_vars;
        BWKeyPair get_vars;
        BWKeyPair file_paths;
        BWKeyPair cookies;
        BWLog m_log;
    public:
        BWHttpRequest(char **envp);
        ~BWHttpRequest();

        BWString GetServerVar(const BWString &var);
        BWString GetParameter(const BWString &var);
        BWString GetPOSTParameter(const BWString &var);
        BWString GetGETParameter(const BWString &var);

        BWString GetFilePath(const BWString &file);
        BWString GetCookie(const BWString &cookie);
        void UnsetParameter(const BWString &parm);
        BWKeyPair& GetServerVars();
        BWKeyPair& GetGetVars();
        BWKeyPair& GetPostVars();
        BWKeyPair& GetCookieVars();
        BWKeyPair GetRequestVars();
        BWString GetRawData();

};

}

#endif

