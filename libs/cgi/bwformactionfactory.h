//
// C++ Implementation: bwformactionfactory
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2013
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef BWFORMACTIONFACTORY_H
#define BWFORMACTIONFACTORY_H

#include <bwformaction.h>
#include <bwreportaction.h>
// #include <bwclientaction.h>
#include <bwdatabase.h>

using namespace CGI;

class BWFormActionFactory {
public:
	static BWFormAction *Create(const BWString &, BWDb *);
};

#endif /* BWFORMACTIONFACTORY_H */
