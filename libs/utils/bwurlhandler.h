//
// C++ Interface: bwurlhandler
//
// Description: 
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWURLHANDLER_H
#define BWURLHANDLER_H

#include <curl/curl.h>
#include <bwstring.h>
#include <bwurl.h>
#include <bwerror.h>
#include <iostream>

using namespace std;

/**
Classe para comunicação com servidores remotos (HTTP, HTTPs, FTP, etc.)

	@author Samuel da Silva Feitosa <samuel@beanweb.com.br>
*/
class BWUrlHandler{
public:
    BWUrlHandler();
    BWUrlHandler(const BWString &url);
    BWUrlHandler(BWUrl &url);

    ~BWUrlHandler();

    void SetUrl(const BWString &url);
    void SetUrl(BWUrl &url);

    BWString Post(const BWString &);
    BWString Post(BWKeyPair &);
    void PostToFile(const BWString &file, const BWString &);
    BWString Get();
    void GetToFile(const BWString &file);
    void EnableCookies();

    static size_t string_callback(void *ptr, size_t size, 
				size_t nmemb, void *usrp);
    static size_t file_callback(void *ptr, size_t size, 
				size_t nmemb, void *usrp);
                
    BWString EscapeString(const BWString &);

private:
    void init();
    void throwErrorByCode(CURLcode c);

    CURL *m_curl;
    BWString m_data;
    BWString m_response;
    FILE *m_fileout;
    BWUrl m_url;
};

#endif
