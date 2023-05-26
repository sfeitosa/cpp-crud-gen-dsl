#define __BWERROR_CPP
#include "bwerror.h"
#include <fstream>
#include <bwlist.h>

using namespace std;

BWError::BWError(bwerror_t sect,
                 const BWPrimString &msg,
                 const BWPrimString &file,
                 int line,
                 bool fatal)
{
  bw_errmsg = msg;
  setErrorPlace(file, line);
  bw_errtype = sect;
  bw_fatal = fatal;
  logError();
}

BWError::~BWError()
{}

/*!
    \fn BWError::setErrorPlace(const BWPrimString &file, int line)
 */
void BWError::setErrorPlace(const BWPrimString &file, int line)
{
  this->bw_whererror = errorPlace(file, line);
}


/*!
    \fn BWError::GetErrorMessage()
 */
BWPrimString BWError::GetErrorMessage() const
{
  return this->bw_errmsg;
}


/*!
    \fn BWError::GetErrorPlace()
 */
BWPrimString BWError::GetErrorPlace() const
{
  return this->bw_whererror;
}

/*!
    @fn BWError::IsFatal()
 */
bool BWError::IsFatal() const
{
  return bw_fatal;
}

/*!
    @fn BWError::IsFatal()
 */
inline void BWError::logError()
{
  fstream logf("/srv/beanweb/var/log/error_log", fstream::app|fstream::out);
  BWPrimString errs("\n########################################################\n");

  errs += error_d[bw_errtype];
  errs += ": ";
  errs += GetErrorPlace() + ": ";

  if (IsFatal())
  {
    errs += "[FATAL] ";
  }


  errs += GetErrorMessage();

  logf << errs << endl;

  logf.close();
}

/*!
    @fn BWError::IsFatal()
 */
BWPrimString BWError::errorPlace(const BWPrimString &file, int line) const
{
  BWPrimString msg;
  BWPrimString nfile;
  const char *p = file.c_str();
  while (*p)
  {
    if (*p == '/')
    {
      nfile = BWPrimString("");
      nfile += *++p;
    }
    else
    {
      nfile += *p++;
    }
  }
  msg = nfile + "";
  msg.AppendInt(line);

  return msg;
}



/*!
    @fn BWError::TrackError(const BWPrimString &file, int line, const BWPrimString &msg = "")
 */
void BWError::TrackError(const BWPrimString &file, int line, const BWPrimString &msg)
{
  fstream logf("/srv/beanweb/var/log/error_log", fstream::app|fstream::out);
  bw_errortrack += "\n\t";
  bw_errortrack += errorPlace(file, line) + ": ";
  bw_errortrack += msg;

  logf << bw_errortrack << endl;

  logf.close();
}

#undef __BWERROR_CPP
