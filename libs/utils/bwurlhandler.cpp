//
// C++ Implementation: bwurlhandler
//
// Description: 
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwurlhandler.h"

BWUrlHandler::BWUrlHandler() {
    init();
}

BWUrlHandler::BWUrlHandler(const BWString &url)
{
    init();

    SetUrl(url);
}

BWUrlHandler::BWUrlHandler(BWUrl &url)
{
    init();

    SetUrl(url);
}

BWUrlHandler::~BWUrlHandler() {
    curl_easy_cleanup(m_curl);
}

void BWUrlHandler::SetUrl(const BWString &url) {
    m_url.SetUrl(url);

    if (!m_curl) {
        throw BWError(BASE_ERROR, 
            "Ponteiro para objeto CURL nao inicializado");
    }

    curl_easy_setopt(m_curl, CURLOPT_URL, m_url.GetUrl().c_str());
}

void BWUrlHandler::SetUrl(BWUrl &url) {
    SetUrl(url.GetUrl());
}

BWString BWUrlHandler::Post(const BWString &data) {
    CURLcode res;

    if (!m_curl) {
        throw BWError(BASE_ERROR, 
            "Ponteiro para objeto CURL nao inicializado");
    }

    curl_easy_setopt(m_curl, CURLOPT_POST, 1L);

    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, string_callback);

    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);

    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data.c_str());

    res = curl_easy_perform(m_curl);

    if (res != 0) {
        throw BWError(APP_ERROR, curl_easy_strerror(res));
    }

    return m_response;
}


BWString BWUrlHandler::Post(BWKeyPair &kpr) {
    BWString postdata;
    BWString response;

    postdata = kpr.ToString("=", "&", "");

    response = Post(postdata);

    return response;
}
 
void BWUrlHandler::PostToFile(const BWString &file, const BWString &data) {
    CURLcode res;

    m_fileout = fopen(file.c_str(), "w");

    if (!m_curl || !m_fileout) {
        throw BWError(BASE_ERROR, 
            "Ponteiro para objeto CURL nao inicializado");
    }

    curl_easy_setopt(m_curl, CURLOPT_POST, 1L);

    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, file_callback);

    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);

    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data.c_str());

    res = curl_easy_perform(m_curl);

    if (res != 0) {
        throw BWError(APP_ERROR, curl_easy_strerror(res));
    }

    fclose(m_fileout);
}

BWString BWUrlHandler::Get() {
    CURLcode res;

    if (!m_curl) {
        throw BWError(BASE_ERROR, 
            "Ponteiro para objeto CURL nao inicializado");
    }

//    curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);

    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, string_callback);

    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);

    res = curl_easy_perform(m_curl);

    if (res != 0) {
        throw BWError(APP_ERROR, curl_easy_strerror(res));
    }

    return m_response;
}

void BWUrlHandler::GetToFile(const BWString &file) {
    CURLcode res;

    m_fileout = fopen(file.c_str(), "wb");

    if (!m_curl || !m_fileout) {
        throw BWError(BASE_ERROR, 
            "Ponteiro para objeto CURL nao inicializado");
    }

    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, file_callback);

    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);

    res = curl_easy_perform(m_curl);

    if (res != CURLE_OK) {
        throw BWError(APP_ERROR, curl_easy_strerror(res));
    }

    fclose(m_fileout);
}

void BWUrlHandler::EnableCookies() {
	curl_easy_setopt(m_curl, CURLOPT_COOKIEFILE, "");
}

size_t BWUrlHandler::string_callback(void *ptr, size_t size, 
				size_t nmemb, void *usrp) {
    size_t realsize = size * nmemb;

    if (!ptr || !usrp) {
        throw BWError(BASE_ERROR, "Ponteiro de dados invalidos");
    }

    BWUrlHandler *obj = static_cast<BWUrlHandler *>(usrp);

    obj->m_response += static_cast<char *>(ptr);

    return realsize;
}

size_t BWUrlHandler::file_callback(void *ptr, size_t size, 
				size_t nmemb, void *usrp) {
    size_t written = 0;

    if (!ptr || !usrp) {
        throw BWError(BASE_ERROR, "Ponteiro de dados invalidos");
    }

    BWUrlHandler *obj = static_cast<BWUrlHandler *>(usrp);

    written = fwrite(ptr, size, nmemb, obj->m_fileout);

    return written;
}

void BWUrlHandler::init(void) {
    curl_global_init(CURL_GLOBAL_ALL);

    m_curl = curl_easy_init();

    if (!m_curl) {
        throw BWError(BASE_ERROR, "Erro ao iniciar objeto CURL");
    }
}

BWString BWUrlHandler::EscapeString(const BWString &orig) {
    BWString dest;
    char *tmp;
    
    tmp = curl_easy_escape(m_curl, orig.c_str(), orig.length());
    
    dest = tmp;
    
    curl_free(tmp);
    
    return dest;
}
