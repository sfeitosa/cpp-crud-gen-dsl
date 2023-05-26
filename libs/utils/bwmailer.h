//
// C++ Interface: bwmailer
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWMAILER_H
#define BWMAILER_H

#include <bwstring.h>
#include <bwlist.h>
#include <bwlog.h>

/**
 Classe de envio de emails

 @author Samuel da Silva Feitosa
*/


class BWMailer {
    public:
        BWMailer();
        ~BWMailer();

        void AddReceiver(const BWString &);
        void AddReceiver(BWList);

        void AddAttachment(const BWString &);
        void AddAttachment(BWList);

        void SetSubject(const BWString &);
        void SetBodyFile(const BWString &);

        void Send();
        void Clear();

    private:
        BWList m_receivers;
        BWList m_attachments;
        BWString m_subject;
        BWString m_bodyfile;
        BWLog m_log;
};

#endif

