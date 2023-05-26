//
// C++ Implementation bwurlmail.h
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2014
// 
// Created on: 13/02/2014
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWURLMAIL_H_
#define BWURLMAIL_H_

#include <curl/curl.h>
#include <bwerror.h>
#include <bwstring.h>
#include <bwlist.h>

class BWUrlMail {
    public:
        BWUrlMail();
        ~BWUrlMail();

        void SetCredentials(const BWString &user, const BWString &pass);
        void SetUrlServer(const BWString &url);
        void SetSender(const BWString &from);
        void AddReceiver(const BWString &to);
        void AddReceiver(BWList recv);
        void SetSubject(const BWString &subj);
        void SetBodyMsg(const BWString &msg);
        void AppendBodyMsg(const BWString &msg);
        BWList GetRawData();

        void Send();

        static size_t send_callback(void *ptr, size_t size, size_t nmemb, void *usrp);

    private:
        void init();
        void prepareMsg();
        void throwErrorByCode(CURLcode c);

        CURL *m_curl;
        BWString m_username;
        BWString m_password;
        BWString m_url;
        BWString m_sender;
        BWList m_receivers;
        BWString m_subject;
        BWString m_bodymsg;
        BWList m_rawmsg;
        unsigned int m_current;
};

#endif /* BWURLMAIL_H_ */
