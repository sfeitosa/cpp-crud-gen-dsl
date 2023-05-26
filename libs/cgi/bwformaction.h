#ifndef BWFORMACTION_H
#define BWFORMACTION_H

#include <bwkeypair.h>
#include <bwlog.h>

namespace CGI {

class BWFormAction {
public:
	BWFormAction();
	virtual ~BWFormAction();

	virtual BWKeyPair Exec(const BWString &action, BWKeyPair &data) = 0;
protected:
	BWLog m_log;
};

} /* namespace cgi */

#endif /* BWFORMACTION_H */
