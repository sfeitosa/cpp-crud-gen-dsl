#ifndef BWERROR_H
#define BWERROR_H

#include <bwprimstring.h>
#include <exception>


typedef enum {
  DB_ERROR = 0,
  HTTP_ERROR,
  CGI_ERROR,
  BASE_ERROR,
  APP_ERROR,
  STRUCTURAL_ERROR,
  SOCK_ERROR,
  WIN_ERROR,
  UNKNOWN_ERROR } bwerror_t;
#ifdef __BWERROR_CPP
const char *error_d[] = {
                    "DB_ERROR",
                    "HTTP_ERROR",
                    "CGI_ERROR",
                    "BASE_ERROR",
                    "APP_ERROR",
                    "STRUCTURAL_ERROR",
                    "SOCK_ERROR",
					"WIN_ERROR",
                    "UNKNOWN_ERROR" };
#endif


class BWError
{
  public:
    //BWError ( const BWPrimString &msg, int code = 0, const BWPrimString &plc="::(NONE)" );
    BWError(bwerror_t sect,
            const BWPrimString &msg,
            const BWPrimString &file,
            int line,
            bool fatal = false);

    ~BWError();
    BWPrimString GetErrorMessage() const;
    BWPrimString GetErrorPlace() const;
    bool IsFatal() const;
    inline void logError();
    void TrackError(const BWPrimString &file, int line, const BWPrimString &msg = "");

  private:
    BWPrimString bw_errmsg;
    BWPrimString bw_errortrack;
    BWPrimString bw_whererror;
    bwerror_t bw_errtype;
    bool bw_fatal;
  protected:
    void setErrorPlace(const BWPrimString &file, int line);
    BWPrimString errorPlace(const BWPrimString &file, int line) const;
};

class BWErrorE : public BWError {
            public:
            BWErrorE(bwerror_t sect,
                     const BWPrimString &msg,
                     const BWPrimString &file,
                     int line,
                     bool fatal = false) :
            BWError(sect, msg, file, line, fatal) 
            {
            }
};

#ifndef __BWERROR_CPP
#define m_BWErrorPlace __FILE__ ":", __LINE__
#define __Error(ERROR_T, MSG, FAT) BWErrorE(ERROR_T, MSG, m_BWErrorPlace, FAT)
#define BWFatalError(ERROR_T, MSG) __Error(ERROR_T, MSG, true)
#define BWError(ERROR_T, MSG) __Error(ERROR_T, MSG, false)
#endif

#endif
