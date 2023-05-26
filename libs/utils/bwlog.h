//
// C++ Interface: bwlog
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWLOG_H
#define BWLOG_H

#include <iostream>
#include <fstream>
#include <ctime>
#include <bwstring.h>

using std::ofstream;
using std::ifstream;

namespace bwlog {
	static const char endl = '\n';
}

/**
Classe responsavel pela manipulacao de arquivos de log.

	@author Samuel da Silva Feitosa <samuel@beanweb.com.br>
*/

class BWLog {
    public:
	enum Severity {
		MSG_ERROR = 0,
		MSG_WARN,
		MSG_INFO,
		MSG_DEBUG,
		MSG_XDEBUG,
		MSG_NONE
	};

        BWLog();
        BWLog(const BWString &path);

        ~BWLog();
        void SetLog(const BWString &path);
        void SetLogLevel(Severity sev);
        void Reopen();

        BWLog &operator()(Severity sev = MSG_INFO);
        BWLog &operator<<(const BWString &rhs);
        BWLog &operator<<(int rhs);
        BWLog &operator<<(const char rhs);
        BWLog &operator<<(float rhs);
        BWLog &operator<<(double rhs);

    private:
        void initLogLine(Severity sev);
        BWString getSeverity(Severity sev);

        static ofstream m_out;
        static Severity m_loglevel;
        static bool m_islogging;
        static BWString m_path;
};

#endif

