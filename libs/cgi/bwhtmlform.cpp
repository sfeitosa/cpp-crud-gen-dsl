//
// C++ Implementation: bwhtmlform
//
// Description:
//
//
// Author: Bruno Moreira Guedes <bruno@beanweb.com.br>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwhtmlform.h"
#include <utils.h>

#define DUMP(VAR) (cerr << #VAR << ": " << VAR << endl)
#define DUMPI(VAR,INDEX) (cerr << #VAR << "[" << #INDEX << "=" << INDEX << "]=" << VAR[INDEX] << endl)

#define HDUMP(S, VAR) ((*S) << "<P>" #VAR ": " << VAR << "</P>")
#define HDUMPI(S, VAR, INDEX) ((*S) << "<P>" #VAR "[" #INDEX "=" << INDEX << "]=" << VAR[INDEX] << "</P>")

// fstream err("/tmp/bhf.out", fstream::out|fstream::app);

namespace CGI
{
  using namespace Http;

  BWHtmlForm::BWHtmlForm(const BWString &file, BWHttp *channel)
  {
    BWConfig cfg;
    cfg.Load(file);
    
    init(cfg, channel);
  }

  BWHtmlForm::BWHtmlForm(BWConfig &cfg, BWHttp *channel)
  {
    init(cfg, channel);
  }

  BWHtmlForm::~BWHtmlForm()
{
            if (bw_config["name"] != "(null)" && bw_config["name"] != "") {
                        BWString compilado_nome("/tmp/bwhtmlform_");
                        compilado_nome += bw_config["name"] + ".cini";
                        std::fstream inicompilado(compilado_nome.c_str(), std::fstream::out);
                        BWKeyPair::iterator itr = bw_config.begin();
//                         cerr << "Abrindo " << compilado_nome << "..." << endl;
                        
                        while(itr != bw_config.end()) {
                                    inicompilado << itr->first << "=" << itr->second << endl;
                                    itr++;
                        }
                        inicompilado.flush();
                        inicompilado.close();
            }
}
}


/*!
    @fn CGI::BWHtmlForm::GetFieldValues()
 */
BWKeyPair CGI::BWHtmlForm::GetFieldValues()
{
  unsigned int i;
  BWKeyPair ret;
  BWString cur;

  for (i = 0; i < bw_fields.size(); i++)
  {
    cur = bw_fields[i];
    ret[cur] = this[0][cur];
  }

  return ret;
}


/*!
    \fn CGI::BWHtmlForm::operator[](const BWString &idx)
 */
BWString CGI::BWHtmlForm::operator[](const BWString &idx)
{
  BWString mfields = bw_config[idx + ".multifields"];
  BWList subfields;
  unsigned int i;
  BWString ret;
//   err << "operator[]: aqui estou" << endl;

  if (mfields != "(null)")
  {
//     err << "novamente..." << endl;
    subfields = mfields.Split(",");
    ret = subfields[0];
//     err << "ret(inicial): " << ret << endl;
    for (i = 1; i < subfields.size(); i++)
    {
//       err << "i=" << i << ", size=" << mfields.size() << endl;
      ret += ",";
      ret += bw_channel->GetParameter(subfields[i]);
//       err << "ret: " << ret << endl;
    }
//     err << "ret(final): " << ret << endl;
  }
  else
  {
    ret = bw_channel->GetParameter(idx);
  }
//   err << "operator[]: retornando" << endl;

  return ret;
}


/*!
    \fn CGI::BWHtmlForm::IsFilled()
 */
bool CGI::BWHtmlForm::IsFilled()
{
  unsigned int i;
  BWList mandatory = bw_config["mandatory"].Split(",");

  if (bw_channel->GetServerVar("REQUEST_METHOD") != "POST")
  {
    return false;
  }
  for (i = 0; i < mandatory.size(); i++)
  {
    if (this[0][mandatory[i]] == "(null)")
    {
      return false;
    }
  }

  return true;
}


/*!
    \fn CGI::BWHtmlForm::AddField(const BWString &name, const BWString &type, const BWString &label, const BWString &default)
 */
void CGI::BWHtmlForm::AddField(const BWString &name, const BWString &type, const BWString &label, const BWString &defaultv)
{
  if (name == "(null)")
  {
    throw BWError(HTTP_ERROR, "Nao foi possivel adicionar campo com nome vazio");
  }
  bw_config["fields"] += ",";
  bw_config["fields"] += name;
  bw_fields.push_back(name);

  bw_config[name + ".type"] = type;
  bw_config[name + ".label"] = label;
  bw_config[name + ".default"] = defaultv;
}


/*!
    \fn CGI::BWHtmlForm::Send()
 */
void CGI::BWHtmlForm::Send()
{
  unsigned int i;
  *bw_channel << "<FIELDSET><LEGEND>" + bw_config["label"] + "</LEGEND>";
  *bw_channel << "<form action='";
  *bw_channel << bw_channel->GetServerVar("REQUEST_URI");
  *bw_channel << "' method='post' name='";
  *bw_channel << bw_formname + "'>\n";
  *bw_channel << "<TABLE>\n";

  for (i = 0; i < bw_fields.size(); i++)
  {
//     *bw_channel << "<B>" << bw_fields[i] << "</B>";
    *bw_channel << GetFieldText(bw_fields[i]);
  }
  
  *bw_channel << "\n</TABLE>\n</BR>";
  *bw_channel << "<BUTTON id='formato' TYPE='submit'>";
  *bw_channel << bw_btn_ok << "</BUTTON>\n";
  *bw_channel << "<BUTTON id='formato' TYPE='reset'>";
  *bw_channel << bw_btn_cancel << "</BUTTON>\n</FORM>\n</FIELDSET>\n";
}

/*!
    \fn CGI::BWHtmlForm::SetChannel(Http::BWHttp& channel)
 */
void CGI::BWHtmlForm::SetChannel(Http::BWHttp* channel)
{
  bw_channel = channel;
}


/*!
    \fn CGI::BWHtmlForm::SetConfig(const BWString &parm, const BWString &val)
 */
void CGI::BWHtmlForm::SetConfig(const BWString &parm, const BWString &val)
{
  bw_config[parm] = val;
}


/*!
    \fn CGI::BWHtmlForm::SetFieldParm(BWString &field, const BWString &parm, const BWString &value)
 */
void CGI::BWHtmlForm::SetFieldParm(BWString &field, const BWString &parm, const BWString &value)
{
  bw_config[field + "." + parm] = value;
}


/*!
    \fn CGI::BWHtmlForm::htmlFormGetFieldText(const BWString &field)
 */
BWString CGI::BWHtmlForm::htmlFormGetFieldText(const BWString &field)
{
            BWString templf(_BW_HTML_FIELDTYPES);
            BWTemplate tpl;
            BWList parms;

            templf += bw_config[field + ".type"] + ".fft";

            tpl.FromFile(templf);

            tpl.SetAttribute("FIELD_TYPE", bw_config[field + ".type"]);
            tpl.SetAttribute("FIELD_LABEL", bw_config[field + ".label"]);
            tpl.SetAttribute("FIELD_NAME", field);
            if (this[0][field] == "(null)" || this[0][field] == "")
            {
                        tpl.SetAttribute("FIELD_VALUE", bw_config[field + ".default"]);
            }
            else
            {
                        tpl.SetAttribute("FIELD_VALUE", this[0][field]);
            }

            if (bw_config[field + ".ids"] != "(null)" && bw_config[field + ".descrs"] != "(null)")
            {
                        tpl.SetAttributeList("FIELDOPT_ID",
                                             (parms = bw_config[field + ".ids"].Split(",")));
                        tpl.SetAttributeList("FIELDOPT_DESCR",
                                             (parms = bw_config[field + ".descrs"].Split(",")));
            }
            if (bw_config[field + ".multilabels"] != "(null)")
            {
                        tpl.SetAttributeList("FIELD_SUBLABEL",
                                             (parms = bw_config[field + ".multilabels"].Split(",")));
            }
            if (bw_config[field + ".multifields"] != "(null)")
            {
                        tpl.SetAttributeList("FIELD_SUBFIELD",
                                             (parms = bw_config[field + ".multifields"].Split(",")));
            }
            if (bw_config[field + ".inputsep"] != "(null)")
            {
                        tpl.SetAttribute("FIELD_INPUTSEP", bw_config[field + ".inputsep"]);
            }

            return tpl.GetText();
}


/*!
    \fn CGI::BWHtmlForm::init(BWConfig &cfg, BWHttp *channel)
 */
void CGI::BWHtmlForm::init(BWConfig &cfg, BWHttp *channel)
{
    BWList mandatory;
    BWString curlabel;
    unsigned int i;

    bw_channel = channel;
    bw_config = cfg;
    bw_btn_ok = "Salvar";
    bw_btn_cancel = "Cancelar";
    
    AddField(bw_config["name"] + "_id__", "hidden", "", "");

    mandatory = bw_config["mandatory"].Split(",");
    for (i = 0; i < mandatory.size(); i++)
    {
      curlabel = bw_config[mandatory[i] + ".label"];
      curlabel += "*";
      bw_config[mandatory[i] + ".label"] = curlabel;
    }

    bw_fields = bw_config["fields"].Split(",");
}
