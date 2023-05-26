#include "bwkeylist.h"

/*!
    @fn BWKeyList::BWKeyList()
    @brief Constroi a classe de manipulacao de lista de chaves
 */
BWKeyList::BWKeyList() : __BASIC_KEYLIST()
{}

/*!
    @fn BWKeyList::~BWKeyList()
    @brief Destroi a classe de manipulacao de lista de chaves
 */
BWKeyList::~BWKeyList()
{}




/*!
    @fn BWKeyList::GetString(const BWString &fld, int idx)
    @param fld Nome do campo a ser obtido
    @param idx Numero do item
    @brief Retorna em forma de string o campo fld na posicao idx.
 */
BWString BWKeyList::GetString(const BWString &fld, int idx)
{
  return this[0][fld][idx];
}


/*!
    @fn BWKeyList::GetInt(const BWString &fld, int idx)
    @param fld Nome do campo a ser obtido
    @param idx Numero do item
    @brief Retorna em forma de inteiro o campo fld na posicao idx.
 */
int BWKeyList::GetInt(const BWString &fld, int idx)
{
  return GetString(fld, idx).GetInt();
}
