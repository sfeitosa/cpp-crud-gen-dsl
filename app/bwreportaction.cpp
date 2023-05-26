//
// C++ Implementation: bwreportaction
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2013
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "bwreportaction.h"

BWReportAction::BWReportAction() : BWFormAction() {
	// TODO Auto-generated constructor stub

}

BWReportAction::~BWReportAction() {
	// TODO Auto-generated destructor stub
}

BWKeyPair BWReportAction::Exec(const BWString &action, BWKeyPair &data) {
    BWKeyPair resp;
    BWString cmd;
    BWString msg;

    m_log(BWLog::MSG_XDEBUG) << "ReportAction::Exec" << bwlog::endl;

    cmd  = "java -jar /srv/mestrado/sbd/swmanager/bin/swmreports.jar ";
    cmd += data["registry_id"];
    cmd += " ";
    cmd += data["__beanweb_dbname"];
    cmd += " &";

    m_log(BWLog::MSG_XDEBUG) << "ReportAction::cmd: " << cmd << bwlog::endl;

    if (fork() == 0) {
        close(0);
        close(1);
        close(2);
        if (system(cmd.c_str()) == -1) {
            m_log(BWLog::MSG_WARN) << "Error running cmd: " << cmd << bwlog::endl;
        }
        exit(0);
    }

    m_log(BWLog::MSG_XDEBUG) << "ReportAction::End of Exec" << bwlog::endl;

    return resp;
}
