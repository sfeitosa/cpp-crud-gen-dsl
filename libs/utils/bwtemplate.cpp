//
// C++ Implementation: bwtemplate
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwtemplate.h"
#include <fstream>

using namespace std;

BWTemplate::BWTemplate()
{}


/*!
    \fn BWTemplate::BWTemplate(const BWKeyPair &data)
 */
BWTemplate::BWTemplate(BWKeyPair &data)
{
  SetAttributes(data);
  EnableNullReplacement();
}


/*!
    \fn BWTemplate::BWTemplate(const BWString &s, const BWKeyPair &data)
 */
BWTemplate::BWTemplate(const BWString &s, BWKeyPair &data)
{
  SetText(s);
  SetAttributes(data);
  EnableNullReplacement();
}


BWTemplate::~BWTemplate()
{}


/*!
    \fn BWTemplate::parseAndReplace()
 */
void BWTemplate::parseAndReplace()
{
  BWList tokens, subtokens, fortokens, forsubtokens, replacelist;
  unsigned int i, j, k, foreach_iterations, curlistsize;
  BWString cur_token, forcur_token, repstr, repappstr, repbystr;
  BWString post_str;

  if (bw_parsed)
  {
    return;
  }

  tokens = bw_skel.Split("<%!foreach");
  if (tokens.size() > 1)
  {
    for (i = 0; i < tokens.size(); i++)
    {
      cur_token = tokens[i];
      subtokens = cur_token.Split("!%>", 2);

      if (subtokens.size() <= 1)
      {
        post_str += cur_token;
        continue;
      }

//             cout << "post_str: " << post_str << endl;

      cur_token = subtokens[0];
      foreach_iterations = 0;

      fortokens = cur_token.Split("<%#");
      for (j = 0; j < fortokens.size(); j++)
      {
        forcur_token = fortokens[j];
        forsubtokens = forcur_token.Split("#%>",2);
        if (forsubtokens.size() <= 1)
        {
          bw_finaltext += cur_token;
          continue;
        }
        curlistsize = bw_listlengths.GetInt(forsubtokens[0]);
        if (curlistsize > foreach_iterations)
        {
          foreach_iterations = curlistsize;
        }
//                         cout << "replacelist: +" << forsubtokens[0] << endl;
        replacelist.push_back(forsubtokens[0]);
      }

      for (j = 1; j <= foreach_iterations; j++)
      {
        repappstr = BWString("_").AppendInt(j);
        forcur_token = cur_token;

        for (k = 0; k < replacelist.size(); k++)
        {
          repstr = "<%#";
          repstr += replacelist[k] + "#%>";
          repbystr = "<%@";
          repbystr += replacelist[k] + repappstr + "@%>";
//                                     cout << "Replacing " << repstr << " by " << repstr + repappstr << endl;
          forcur_token.Replace(repstr, repbystr);
//                                     cout << "Replaced!" << endl;
        }
        post_str += forcur_token;
      }
      post_str += subtokens[1];
    }
  }
  else
  {
    post_str = bw_skel;
  }
  /** O seguinte algoritmo foi utilizado por questoes de desempenho e robustez.
      Percorrer com um laço a lista de variaveis e aplicar Replace na string
      poderia simplificar o codigo, mas o torna menos confiavel e causa perdas
      de performance significativas. 
  */
  tokens = post_str.Split("<%@");
  if (tokens.size() <= 1)
  {
    bw_finaltext = post_str;
    return;
  }

  bw_finaltext = "";
  for (i = 0; i < tokens.size(); i++)
  {
    cur_token = tokens[i];
    subtokens = cur_token.Split("@%>", 2);

    if (subtokens.size() <= 1)
    {
      bw_finaltext += cur_token;
      continue;
    }

    if (bw_data[subtokens[0]] != "(null)") {
          bw_finaltext += bw_data[subtokens[0]];
    }
    else {
          if (HasNullReplacement()) {
              bw_finaltext += "(null)";
          }
          else {
              bw_finaltext += "<%@";
              bw_finaltext += subtokens[0] + "@%>";
          }
    }
    bw_finaltext += subtokens[1];
  }
}


/*!
    \fn BWTemplate::GetText()
 */
BWString BWTemplate::GetText()
{
  if (!bw_parsed)
  {
    parseAndReplace();
  }
  return bw_finaltext;
}


/*!
    \fn BWTemplate::SetAttribute(const BWString &key, const BWString &value)
 */
void BWTemplate::SetAttribute(const BWString &key, const BWString &value)
{
  BWString first = key.substr(0,4);
  if (first.ToUpper() == "THIS")
  {
    throw BWError(BASE_ERROR, "'THIS' word is reserved");
  }
  if (value == "(null)") {
    bw_data[key] = "";
  }
  else {
    bw_data[key] = value;
  }
  bw_parsed = false;
}


/*!
    \fn BWTemplate::SetAttributeList(const BWString &attr, BWList &ls)
 */
void BWTemplate::SetAttributeList(const BWString &attr, BWList &ls)
{
  BWString base = attr + "_";
  BWString cur;
  BWString curval;
  unsigned int i;

  for (i = 0; i < ls.size(); i++)
  {
    cur = base;
    curval = ls[i];
    cur.AppendInt(i+1);
    SetAttribute(cur, curval);
  }

  bw_listlengths[attr] = BWString().AppendInt(ls.size());
}


/*!
    \fn BWTemplate::SetAttributes(BWKeyPair &kpr)
 */
void BWTemplate::SetAttributes(BWKeyPair &kpr)
{
  BWKeyPair::iterator i;

  for (i = kpr.begin(); i != kpr.end(); i++)
  {
    SetAttribute(i->first, i->second);
  }
}


/*!
    \fn BWTemplate::SetText(const BWString &text)
 */
void BWTemplate::SetText(const BWString &text)
{
  if (bw_skel != text)
  {
    bw_skel = text;
    bw_parsed = false;
  }
}


/*!
    \fn BWTemplate::EnableThrow()
 */
void BWTemplate::EnableThrow()
{
  bw_nullthrow = true;
}


/*!
    \fn BWTemplate::DisableThrow()
 */
void BWTemplate::DisableThrow()
{
  bw_nullthrow = false;
}


/*!
    \fn BWTemplate::FromFile(const BWString &file)
 */
void BWTemplate::FromFile(const BWString &file)
{
  fstream inf(file.c_str(), fstream::in);
  BWString buff;
  bw_skel = "";

  while (getline(inf, buff))
  {
    buff += "\n";
    bw_skel += buff;
  }
}


/*!
    \fn BWTemplate::ToFile(const BWString &file)
 */
void BWTemplate::ToFile(const BWString &file)
{
  fstream outf(file.c_str(), fstream::out);

  parseAndReplace();

  outf << bw_finaltext;
}



/*!
    \fn BWTemplate::ClearAttributes()
 */
void BWTemplate::ClearAttributes()
{
    bw_data.clear();
    bw_parsed = false;
}


/*!
    \fn BWTemplate::HasNullReplacement(void)
 */
bool BWTemplate::HasNullReplacement(void)
{
    return bw_nullreplacement;
}


/*!
    \fn BWTemplate::DisableNullReplacement()
 */
void BWTemplate::DisableNullReplacement()
{
    bw_nullreplacement = false;
}


/*!
    \fn BWTemplate::EnableNullReplacement()
 */
void BWTemplate::EnableNullReplacement()
{
    bw_nullreplacement = true;
}

/*!
    \fn BWTemplate::EnableNullReplacement()
 */
void BWTemplate::ReplaceText(const BWString &key, const BWString &value)
{
	BWString rep = "<%&" + key + "&%>";
    bw_skel.Replace(rep, value);
}
