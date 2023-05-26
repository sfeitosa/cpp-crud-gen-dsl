//
// C++ Interface: bwutils
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef __BWUTILS_H
#define __BWUTILS_H

#include <bwstring.h>
#include <bwlist.h>
#include <sys/types.h>

#if defined(linux) || defined(__linux__)
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#else
//#include <windows.h>
#include <bwwindefs.h>
#include <time.h>
#endif

#define BWMASK_DTNOW "%Y-%m-%d %H:%M:%S"

void exec_sleep(int sec);
void exec_usleep(int usec);
int exec_getcurpid();
BWString get_date(const BWString &mask);
BWString get_pc_name();

#endif
