#include <bwform.h>
#include <bwhttp.h>
#include <bwerror.h>
#include <bwdbsqlite.h>
#include <bwdb.h>
#include <bwresultset.h>
#include <bwstring.h>

using namespace Http;

int validate_session(BWDb *banco, const BWString &sessid);
int get_session_status(BWDb *banco, const BWString &iporig);

