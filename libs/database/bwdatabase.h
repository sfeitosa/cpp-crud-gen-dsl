#ifndef __BWDATABASE_H
#define __BWDATABASE_H

#include <bwurl.h>
#include <bwdb.h>
#include <bwdbregistry.h>
#include <bwresultset.h>
#include <bwconfig.h>
#include <bwdbload.h>

BWDb *bwdb_load(const BWString &rcpath, bool justload = false);

#endif//__BWDATABASE_H
