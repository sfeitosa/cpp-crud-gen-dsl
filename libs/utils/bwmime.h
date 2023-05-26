//
// C++ Interface: bwmime
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWMIME_H
#define BWMIME_H

#include <fstream>
#include <bwstring.h>
#include <bwlist.h>
#include <bwlog.h>

using std::fstream;

/**
 Classe para fazer parsing em arquivos de email

 @author Samuel da Silva Feitosa
*/


class BWMime {
    public:
        BWMime();
        BWMime(const BWString &file);
        ~BWMime();

        void SetFile(const BWString &);
        BWList GetAttachments();
        BWList GetAttachExtensions();

    private:
        void parseMailFile();

        BWString m_mailfile;
        BWList m_attachments;
        BWList m_attachextensions;
        bool m_parsed;
        BWLog m_log;
};

#endif

