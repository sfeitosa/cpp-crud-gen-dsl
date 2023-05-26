#ifndef BWMIME_H
#define BWMIME_H

#include <fstream>
#include <bwstring.h>
#include <bwlist.h>
#include <bwlog.h>

using std::fstream;

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

