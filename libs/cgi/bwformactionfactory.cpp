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

#include "bwformactionfactory.h"

BWFormAction *BWFormActionFactory::Create(const BWString &obj, BWDb *db) {
	BWFormAction *act = NULL;

	if (obj == "report") {
		act = new BWReportAction();
	}
	// else if (obj == "client") {
	//     act = new BWClientAction(db);
	// }
	else {
		throw BWError(APP_ERROR, "Criação de manipulador inválido: [" + obj + "]");
	}

	return act;
}

