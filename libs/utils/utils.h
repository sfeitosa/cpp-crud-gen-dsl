#ifndef __UTILS_H
#define __UTILS_H


#include <bwstring.h>
#include <bwkeypair.h>
#include <bwlist.h>
#include <fstream>
#include <iostream>

using namespace std;

BWString urlencode(const BWString &s);

BWString urldecode(const BWString &s);

//BWKeyPair envget();
BWKeyPair envget(char **envp);

BWKeyPair load_config(const BWString &cf);

BWKeyPair getqs(const BWString &qs);

#endif//__UTILS_H
