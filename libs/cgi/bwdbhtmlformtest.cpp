#include <bwdatabase.h>
#include <bwstring.h>
#include <bwdbhtmlform.h>
#include <bwhttp.h>

int main(int argc, char **argv, char **envp)
{
  BWHttp cgi(envp);
  BWDb dbrc = bwdb_load("dbconfig_dbhtmlformtest.ini");
  BWDbHtmlForm form("dbhtmlformtest.ini", dbrc);

  cgi << "<HTML>";
  cgi << " <HEAD>";
  cgi << "  <TITLE>Teste BWDbHtmlForm</TITLE";
  cgi << " </HEAD>";
  cgi << " <BODY>";

  form.Send(cgi);

  cgi << " </BODY>";
  cgi << "</HTML>";

  return 0;
}

