#include "bwlog.h"

ofstream BWLog::m_out;
BWLog::Severity BWLog::m_loglevel;
bool BWLog::m_islogging;
BWString BWLog::m_path;

BWLog::BWLog()
{
}


BWLog::~BWLog()
{
}

/*!
    \fn BWLog::BWLog(const BWString &path)
 */
BWLog::BWLog(const BWString &path)
{
	SetLog(path);
}


/*!
    \fn BWLog::SetLog()
 */
void BWLog::SetLog(const BWString &path)
{
	m_path = path;

	m_out.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);

	try {
		m_out.open(path.c_str(), ofstream::app);
	}
	catch (ofstream::failure &e) {
		std::cerr << "Erro ao abrir arquivo de log." << std::endl;
		throw BWError(BASE_ERROR, "Impossivel abrir arquivo de log.");
	}

	m_loglevel = MSG_INFO;
	m_islogging = false;
}

void BWLog::Reopen() {
	if (m_path == "") {
		return;
	}

	try {
		if (m_out.is_open()) {
			m_out.close();
		}
		m_out.open(m_path.c_str(), ofstream::app);
	}
	catch (ofstream::failure &e) {
		std::cerr << "Erro ao reabrir arquivo" << std::endl;
		throw BWError(BASE_ERROR, "Impossivel reabrir arquivo.");
	}
}

void BWLog::SetLogLevel(Severity sev) {
	m_loglevel = sev;
	m_islogging = false;
}

BWLog &BWLog::operator()(Severity sev) {
	if (m_out.is_open() && m_loglevel >= sev) {
		m_islogging = true;
		initLogLine(sev);
	}
	else {
		m_islogging = false;
	}

	return *this;
}

BWLog &BWLog::operator<<(const BWString &rhs) {
	if (m_out.is_open() && m_islogging) {
		m_out << rhs;
		m_out.flush();
	}

	return *this;
}

BWLog &BWLog::operator<<(int rhs) {
	if (m_out.is_open() && m_islogging) {
		m_out << rhs;
		m_out.flush();
	}

	return *this;
}

BWLog &BWLog::operator<<(const char rhs) {
	if (m_out.is_open() && m_islogging) {
		m_out << rhs;
		m_out.flush();
	}

	return *this;
}

BWLog &BWLog::operator<<(float rhs) {
	if (m_out.is_open() && m_islogging) {
		m_out << rhs;
		m_out.flush();
	}

	return *this;
}

BWLog &BWLog::operator<<(double rhs) {
	if (m_out.is_open() && m_islogging) {
		m_out << rhs;
		m_out.flush();
	}

	return *this;
}

void BWLog::initLogLine(Severity sev) {
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];
	
	time(&rawtime);

	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "[%d/%m/%Y %H:%M:%S] ", timeinfo);

	m_out << buffer << "(" << getSeverity(sev) << "): ";
}

BWString BWLog::getSeverity(Severity sev) {
	switch (sev) {
		case 	MSG_ERROR:	return "ERROR";
		case 	MSG_WARN:	return "WARN";
		case 	MSG_INFO:	return "INFO";
		case 	MSG_DEBUG:	return "DEBUG";
		case 	MSG_XDEBUG:	return "XDEBUG";
		default:		return "NONE";
	}
}

