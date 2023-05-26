#ifndef BWUPDATER_H
#define BWUPDATER_H

#include <bwstring.h>
#include <bwconfig.h>
#include <bwkeypair.h>
#include <bwurlhandler.h>
#include <bwlog.h>
#include <bwutils.h>
#include <bwhashmd5.h>
#include <bwprocess.h>

#define BW_TMPDIR "/srv/beanweb/var/tmp/"

class BWUpdater {
    public:
        BWUpdater();
        BWUpdater(BWConfig &, const BWString &, const BWString &);
        ~BWUpdater();

        void SetInfo(const BWString &, const BWString &);
        void SetConfig(BWConfig &);
        bool HasUpdates();
        bool DownloadUpdate();
        BWString GetEvalTime();
        void PrepareStartupInstall();
        void InstallUpdate();

    private:
        BWKeyPair m_updinfo;
        BWConfig m_cfg;
        BWString m_swname;
        BWString m_swversion;
        BWLog m_log;
};

#endif // BWUPDATER_H
