#include "bwstring.h"
#include <iostream>
/*!
    @fn BWString::BWString()
    @brief Constroi a classe de manipulacao de string
 */
BWString::BWString() : BWPrimString()
{}

/*!
    @fn BWString::BWString(const char *s)
    @brief Constroi a classe de manipulacao de string apartir de um char *
 */
BWString::BWString(const char *s) : BWPrimString(s)
{}

BWString::BWString(const BWString &s) : BWPrimString(s)
{}

BWString::BWString(const BWPrimString &s) : BWPrimString(s)
{}

BWString::BWString(const __BASIC_STRING &s) : BWPrimString(s)
{}

/*!
    @fn BWString::GetInt()
    @brief Destroi a classe de manipulacao de string
 */
BWString::~BWString()
{}

BWPrimList BWString::Split(const BWPrimString &sep, int limit) const
{
//             std::cout << "Split()" << std::endl;
  BWPrimList ret;
  BWString cur=this[0];
  BWString sub;
  int pos;
  bool limit_isset = false;
  if (limit > 0)
  {
    limit_isset = true;
  }

  const int sepsize = sep.size();


  while ((pos=cur.find(sep)) >=0)
  {
    if (limit_isset)
    {
//                                     std::cerr << "limit=" << limit << std::endl;
      if (!--limit)
      {
        break;
      }
    }
    sub = cur.substr(0, pos);
//                         std::cerr << "sub=" <<sub << std::endl;
    ret.push_back(sub);

    pos += sepsize;

    cur = cur.substr(pos, (cur.size() - pos));
//                         std::cerr << "new cur="<<cur<<std::endl;
  }

  ret.push_back(cur);    //adiciona o ultimo token
//             std::cerr << "pos=" <<pos << std::endl;

  if (ret.size() == 0)
  {
    ret.push_back(this[0]);
  }

//             std::cerr << "END" << std::endl;
  return ret;
}


/*!
    \fn BWString::operator=(const BWString &s)
 */
BWString &BWString::operator= (const BWString &s)
{
  //std::cerr << "BWString: Copiando "<<s<<std::endl;
  return static_cast<BWString&>(this->assign(s));
}


/*!
    \fn BWString::Hash()
 */
// BWString BWString::Hash()
// {
//   BWString retstr("");
//   unsigned char *datastr = reinterpret_cast<unsigned char*>(const_cast<char*>(c_str()));
//   unsigned char ret[MD5_DIGEST_LENGTH + 2];
//   char newstr[3];
//   int i;

//   MD5(datastr, size(), ret);

//   for (i = 0; i < MD5_DIGEST_LENGTH; i++)
//   {
//     sprintf(newstr, "%2x", ret[i]);
//     if (newstr[0] == ' ')
//     {
//       newstr[0] = '0';
//     }
//     if (newstr[1] == ' ')
//     {
//       newstr[1] = '0';
//     }
//     retstr += newstr;
//   }

//   return retstr;
// }

BWString BWString::Hash()
{
  BWString retstr("");
  unsigned char *datastr = reinterpret_cast<unsigned char*>(const_cast<char*>(c_str()));
  unsigned char ret[EVP_MAX_MD_SIZE + 2];
  unsigned int retsize;
  char newstr[3];
  unsigned int i;

  EVP_Digest(datastr, size(), ret, &retsize, EVP_md5(), NULL);

  for (i = 0; i < retsize; i++)
  {
    sprintf(newstr, "%2x", ret[i]);
    if (newstr[0] == ' ')
    {
      newstr[0] = '0';
    }
    if (newstr[1] == ' ')
    {
      newstr[1] = '0';
    }
    retstr += newstr;
  }

  return retstr;
}


/*!
    @fn BWString::Replace(const BWString &s1, const BWString &s2, int limit = 0)
    @brief Substitui as 'limit' primeiras ocorr�ncias de s1 na string por s2.
    @brief Se limit for zero, substitui todas as ocorr�ncias encontradas
 */
void BWString::Replace(const BWString &s1, const BWString &s2, int limit)
{
  BWPrimList pls;
  BWString replaced;
  unsigned int i, lim;

  pls = this->Split(s1, limit);
  lim = pls.size();
  replaced = pls[0];

  for (i = 1; i < lim; i++)
  {
    replaced += s2;
    replaced += pls[i];
  }

  this[0] = replaced;
}

/*!
    \fn BWString::ToLower()
 */
BWString& BWString::ToLower()
{
  BWString::iterator i;
  for (i = this->begin(); i != this->end(); i++)
    *i = tolower(*i);
  return this[0];
}


/*!
    \fn BWString::ToUpper()
 */
BWString& BWString::ToUpper()
{
  BWString::iterator i;
  for (i = this->begin(); i != this->end(); i++)
    *i = toupper(*i);
  return this[0];
}

BWString &BWString::Escape(const BWString &str, const BWString &esc)
{
  size_t pos = 0;

  do {
    pos = this->find(str, pos);

    if (pos != BWString::npos) {
      this->insert(pos, esc);
      pos = pos + 2;
    }
  } while (pos != BWString::npos);

  return this[0];
}

BWString &BWString::FromFile(const BWString &fname) {
  std::fstream inf(fname.c_str(), std::fstream::in);
  std::string buff = "";

  this->assign(buff);

  while (getline(inf, buff)) {
    this->append(buff);
    this->append("\n");
  }

  return this[0];
}

BWString &BWString::LFilter(const BWString &f) {
  this->erase(0, this->find_first_not_of(f));

  return this[0];
}

BWString &BWString::RFilter(const BWString &f) {
  this->erase(this->find_last_not_of(f) + 1, this->size());

  return this[0];
}

BWString &BWString::LTrim() {
  LFilter(" ");
  LFilter("\n");

  return this[0];
}

BWString &BWString::RTrim() {
  RFilter(" ");
  RFilter("\n");

  return this[0];
}

BWString &BWString::Trim() {
  LTrim();
  RTrim();
  return this[0];
}

BWString &BWString::LPad(int len, char chr) {
    std::ostringstream ss;

    ss << std::setw(len) << std::setfill(chr) << this->c_str();
    *this = ss.str();

    return this[0];
}

void BWString::ToFile(const BWString &fname) {
  std::fstream outf(fname.c_str(), std::fstream::out);
  
  outf << this[0];
  outf.flush();
}
