//
// C++ Implementation: bwdatabase
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <bwdbload.h>

BWDb *bwdb_connect(const BWUrl &u)
{
    HMODULE handler;
    bwdb_load_t driver = NULL;
    BWString libfile = BW_LIBDIR "lib";
    BWString symbol = "bwdb_load_";
	
    libfile += u.GetProtocol() + ".dll";
    symbol  += u.GetProtocol();
	
    handler = LoadLibraryEx((LPSTR) libfile.c_str(), 
                    NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	
    if (!handler) {
        throw BWError(DB_ERROR,
                      BWString("Erro abrindo ") +
                      libfile);
    }
	
    driver = reinterpret_cast<bwdb_load_t>(GetProcAddress(handler, symbol.c_str()));
	
    if (!driver) {
        throw BWError(DB_ERROR,
                      BWString("Erro carregando dados de ") +
                      libfile);
    }
	
    return driver(&u);
}

