//
// C++ Implementation: bwmime
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2012
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwmime.h"

BWMime::BWMime() {
    m_parsed = false;
}

BWMime::BWMime(const BWString &file) {
    m_parsed = false;
    m_mailfile = file;
}

BWMime::~BWMime() {
}

void BWMime::SetFile(const BWString &file) {
    m_mailfile = file;
    m_parsed = false;
}



void BWMime::parseMailFile() {
    fstream stream(m_mailfile.c_str(), fstream::in);
    BWString line;
    BWString attach;
    BWString ext;
    int pos;

    while (getline(stream, line)) {
        // Encontrado anexo
        if (line.find("filename=") != BWString::npos) {
            pos = line.find("=");
            attach = line.substr(pos + 1);
            attach.Replace("\"", "");

            m_attachments.push_back(attach);
            ext = attach.substr(attach.find_last_of(".") + 1);
            m_attachextensions.push_back(ext);
        }
    }

    stream.close();

    m_parsed = true;
}

BWList BWMime::GetAttachments() {
    if (!m_parsed) {
        parseMailFile();
    }

    return m_attachments;
}

BWList BWMime::GetAttachExtensions() {
    if (!m_parsed) {
        parseMailFile();
    }

    return m_attachextensions;
}

