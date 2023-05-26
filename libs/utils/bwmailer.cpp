#include "bwmailer.h"

BWMailer::BWMailer() {
}

BWMailer::~BWMailer() {
}

void BWMailer::Clear() {
    m_receivers.clear();
    m_attachments.clear();
    m_subject = "";
    m_bodyfile = "";
}

void BWMailer::AddReceiver(const BWString &email) {
    m_receivers.push_back(email);
}

void BWMailer::AddReceiver(BWList l) {
    for (unsigned int i = 0; i < l.size(); i++) {
        AddReceiver(l[i]);
    }
}

void BWMailer::AddAttachment(const BWString &path) {
    m_attachments.push_back(path);
}

void BWMailer::AddAttachment(BWList l) {
    for (unsigned int i = 0; i < l.size(); i++) {
        AddAttachment(l[i]);
    }
}

void BWMailer::SetSubject(const BWString &sub) {
    m_subject = sub;
}

void BWMailer::SetBodyFile(const BWString &path) {
    m_bodyfile = path;
}

void BWMailer::Send() {
    BWString cmdline = "mutt";
    int sysret = 0;

    cmdline += " -s \"" + m_subject + "\"";

    for (unsigned int i = 0; i < m_attachments.size(); i++) {
        cmdline += " -a " + m_attachments[i];
    }

    cmdline += " --";

    for (unsigned int i = 0; i < m_receivers.size(); i++) {
        cmdline += " " + m_receivers[i];
    }

    cmdline += " < " + m_bodyfile;

    m_log(BWLog::MSG_DEBUG) << "Executando: " << cmdline << bwlog::endl;

    sysret = system(cmdline.c_str());

    if (sysret == -1) {
        m_log(BWLog::MSG_WARN) << "Sending e-mail failed!" << bwlog::endl;
    }
}

