//
// C++ Interface: bwhttp
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWHTTPBWHTTP_H
#define BWHTTPBWHTTP_H

#include <bwhttprequest.h>
#include <bwhttpresponse.h>

namespace Http
{

  /**
  Classe de manipulacao de HTTP

   @author
  */
  class BWHttp : public BWHttpRequest, public BWHttpResponse
  {
    public:
      BWHttp(char **envp);
      ~BWHttp();
      void SetCookie(const BWString &name, const BWString &valor);

  };

}

#endif
