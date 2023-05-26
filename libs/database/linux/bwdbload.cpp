#include "bwdbload.h"

BWDb *bwdb_connect(const BWUrl &u)
{
  void *handler = NULL;
  bwdb_load_t *driver = NULL;
  BWString libfile = BW_LIBDIR "lib";
  BWString symbol("bwdb_load_"); 

  libfile += u.GetProtocol() + ".so";
  symbol += u.GetProtocol();

  handler = dlopen(libfile.c_str(), RTLD_NOW | RTLD_GLOBAL);

  /*!
   * Mudado de RTLD_LAZY para RTLD_NOW|RTLD_GLOBAL devido ao reportado nos
   * reports de bug numeros 3993 e 4993(http://gcc.gnu.org/bugzilla/show_bug.cgi?id=4993).
   * Como dlopen() nao suporta C++, ele nao carrega as tabelas de simbolos virtuais do C++,
   * e por isso eh preciso este 'workaround'.
   * OBS: compilar com -E -Wl 
    */

  if (!handler)
  {
    throw BWError(DB_ERROR,
                  BWString("Erro abrindo ") +
                  libfile + ": " +
                  dlerror());
  }

  driver = reinterpret_cast<bwdb_load_t*>(dlsym(handler, symbol.c_str()));

  if (!driver)
  {
    throw BWError(DB_ERROR,
                  BWString("Erro carregando dados de ") +
                  libfile + ": " +
                  dlerror());
  }

  return driver(&u);
}

