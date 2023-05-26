#include <bwdatabase.h>
#include <bwkeypair.h>
#include <utils.h>

BWDb *bwdb_load(const BWString &rcpath, bool justload)
{
  BWConfig cfg;
  BWString dburl, cmd, init;
  BWList comands;
  BWDb *ret = NULL;

  cfg.Load(rcpath);

  dburl = cfg["dburl"];
  if (dburl == "" || dburl == "(null)")
  {
    throw BWFatalError(DB_ERROR,
                       "Erro no arquivo de config. de banco de dados: sem dburl");
  }

  try {
            BWUrl url(dburl);
            ret = bwdb_connect(url);
  }
  catch (BWError &e)
  {
              e.TrackError(m_BWErrorPlace, "URL de conexao mal formatada");
              throw e;
  }

  init = cfg["init"];
  if (init == "" || init == "(null)" || justload)
  {
   return ret;
  }
  else
  {
    comands = init.Split(",");

    for (unsigned int i = 0; i < comands.size(); i++)
    {
      cmd = cfg[comands[i]];
      if (cmd != "")
      {
        ret->ExecuteUpdate(cmd);
      }
    }
  }

  return ret;

}
