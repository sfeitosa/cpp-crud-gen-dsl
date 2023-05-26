#include "bwhttpresponse.h"

namespace Http
{

  BWHttpResponse::BWHttpResponse()
  {
    SetHeader("Content-type", "text/html");
    m_data_sent = false;
  }


  BWHttpResponse::~BWHttpResponse()
  {
    SendResponse();
  }

  /*!
      \fn BWHttpResponse::append()
   */
  void BWHttpResponse::append(const BWString &str)
  {
    m_data += str;
  }


  /*!
      \fn BWHttpResponse::Clear()
   */
  void BWHttpResponse::Clear()
  {
    m_data = "";
  }


  /*!
      \fn BWHttpResponse::Reset()
   */
  void BWHttpResponse::Reset()
  {
    BWKeyPair empty_kp;
    BWList empty_list;

    headerl = empty_list;
    headers = empty_kp;

    Clear();
  }


  /*!
      \fn BWHttpResponse::SendResponse()
   */
  void BWHttpResponse::SendResponse()
  {
    BWKeyPair::iterator itr;

    if (!m_data_sent)
    {
      itr = headers.begin();
      while (itr != headers.end())
      {
        cout << itr->first << ": " << itr->second << '\r' << endl;
        itr++;
      }

      itr = cookies.begin();
      while (itr != cookies.end())
      {
        cout << "Set-Cookie: " << itr->first << "=" << itr->second << '\r' << endl;
        itr++;
      }

      cout << '\r' << endl; //Encerra envio de headers
      m_data_sent = true; //Sinaliza que headers ja foram enviados
    }

    if (m_data != "")
    {
      cout << m_data << endl;
      Clear();
    }
  }


  /*!
      \fn BWHttpResponse::SetCookie(const BWString &name, const BWString &valor="")
   */
  void BWHttpResponse::SetCookie(const BWString &name, const BWString &valor)
  {
    unsigned int i;
    int chr;
    BWString non_alnum_valid("_");
    cerr << "Set-Cookie: " << name << "=" << valor << endl;
    if (name == "(null)" || name == "")
    {
                return;
    }

    for (i = 0; i < name.size(); i++)
    {
      chr = static_cast<int>(name[i]);
      if (!isalnum(chr) && non_alnum_valid.find(name[i]) < 0)
      {
        throw BWError(HTTP_ERROR, 
                      "Caractere invalido no nome do cookie");
      }
    }

    cookies[name] = urlencode(valor);
  }


  /*!
      \fn BWHttpResponse::SetHeader(const BWString &header, const BWString &valor)
   */
  void BWHttpResponse::SetHeader(const BWString &header, const BWString &valor)
  {
    if (header != "" && header != "(null)") {
            headers[header] = valor;
    }
  }


  /*!
      \fn BWHttpResponse::GetHeader(const BWString &header)
   */
  BWString BWHttpResponse::GetHeader(const BWString &header)
  {
    if (header != "" || header != "(null)") {
                return headers[header];
    }
    
    return "(null)";
  }


  /*!
      \fn BWHttpResponse::operator<<(const BWString &data)
   */
  BWHttpResponse& BWHttpResponse::operator<< (const BWString &data)
  {
    append(data);
    return *this;
  }


}


/*!
    \fn Http::BWHttpResponse::AppendFile(const BWString &fname)
 */
void Http::BWHttpResponse::AppendFile(const BWString &fname)
{
    fstream file(fname.c_str(), fstream::in);
    BWString line;

    while (getline(file, line)) {
        m_data += line + "\n";
    }
}
