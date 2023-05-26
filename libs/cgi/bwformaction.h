//
// C++ Implementation: bwformaction
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2013
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
