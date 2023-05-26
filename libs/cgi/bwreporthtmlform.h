#ifndef CGIBWREPORTHTMLFORM_H
#define CGIBWREPORTHTMLFORM_H

#define _BW_RELDIR "var/www/html/rel/"
#define _BW_BINDIR "bin/"

#include <bwhtmlform.h>
#include <bwdb.h>
#include <bwstring.h>
#include <bwdbregistry.h>
#include <bwlog.h>

namespace CGI {


class BWReportHtmlForm : public BWHtmlForm
{
public:
    BWReportHtmlForm(const BWString &file, BWHttp *channel, BWDb &db);
    BWReportHtmlForm(BWConfig &cfg, BWHttp *channel, BWDb &db);
    ~BWReportHtmlForm();

    BWString GetFieldText(const BWString &field);
    bool FillReport();

protected:
    BWDb *m_db;

private:
    void init(BWDb &db);
    BWString makeReportName(BWString &report, BWString &type);
    BWString makeRandom();
    BWString makeCmdLine(const BWString &relname, 
    			const BWString &filters, 
			const BWString &logfile);
    void sendReport(BWString &relname, BWString &format);
    BWString getTypeIds(void);
    BWString getTypeDescrs(void);

    BWString m_randcode;
};

}

#endif
