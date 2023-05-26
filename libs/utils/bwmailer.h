#ifndef BWMAILER_H
#define BWMAILER_H

#include <bwstring.h>
#include <bwlist.h>
#include <bwlog.h>

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

