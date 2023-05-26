//
// C++ Interface: bwdatabase
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef __BWDBLOAD_H
#define __BWDBLOAD_H

#include <bwwindefs.h>
#include <bwurl.h>
#include <bwdb.h>

#ifndef BW_LIBDIR
#define BW_LIBDIR "C:\\srv\\beanweb\\lib\\"
#endif

extern "C"
{
  typedef BWDb *(*bwdb_load_t)(const BWUrl *);
  typedef void (*bwdb_unload_t)(const BWDb *);
}

BWDb *bwdb_connect(const BWUrl &u);

#endif //__BWDBLOAD_H
