#include "bwform.h"
#include <iostream>
using namespace std;

namespace Http
{

  BWForm::BWForm(const BWString &nomep, BWHttp *httpp)
  {
    this->http = httpp;
    this->nome = nomep;
    isLoaded = false;
    fnamepre = BWString("__fid_") + nome;
  }


  BWForm::~BWForm()
  {}
}


/*!
    \fn Http::BWForm::operator[](const BWString &field)
 */
BWString Http::BWForm::operator[](const BWString &field)
{
  if (fields[field] == "password")
  {
    return curfieldvalues[field].Hash();
  }
  else
  {
    return curfieldvalues[field];
  }
}


BWString Http::BWForm::GetRawData(const BWString &field) 
{
  return curfieldvalues[field];
}

/*!
    \fn Http::BWForm::IsFilled()
 */
bool Http::BWForm::IsFilled()
{
  unsigned int i = 0;
  loadHttpData();

  if (http->GetServerVar("REQUEST_METHOD") != "POST")
  {
    return false;
  }
  if (http->GetPOSTParameter(fnamepre) == "")
  {
    return false;
  }

  for (i = 0; i < fieldnames.size(); i++)
  {
    BWString fld = fieldnames[i];
    if (curfieldvalues[fld] == "")
    {
      return false;
    }
  }

  return true;
}


/*!
    \fn Http::BWForm::Load(const BWString &fname)
 */
void Http::BWForm::Load(const BWString &fname)
{
  BWString line, load;
  BWList tokens;

  if (fname == "")
  {
    load = nome + ".frm";
  }
  else if (fname[fname.size() - 1] == '/')
  {
    load = fname;
    load += nome + ".frm";
  }
  fstream f(load.c_str(), fstream::in);

  if (f.fail()) {
    throw BWError(BASE_ERROR, "Impossivel abrir arquivo do formulario.");
  }

  while (!f.eof())
  {
    getline(f, line);
    if (line == "")
    {
      continue;
    }
    tokens = line.Split(":");
    cerr << tokens[0] << endl;
    cerr << tokens.size() << endl;
    if (tokens.size() < 4)
    {
      if (label == "" && tokens.size() == 2 && tokens[0] == "__LABEL__")
      {
        label = tokens[1];
        continue;
      }
      else
      {
        throw BWError(HTTP_ERROR, "Formato de arquivo de formulario invalido");
      }
    }
    AddField(tokens[0], tokens[1], tokens[2]);
    if (tokens[3] != "")
    {
      SetLabel(tokens[0], tokens[3]);
    }
  }
  f.close();
}


/*!
    \fn Http::BWForm::Ask()
 */
void Http::BWForm::Ask()
{
  *http << "<form action='" << http->GetServerVar("REQUEST_URI") + "' method='post' name='" +
  nome + "'><fieldset>";
  if (label != "")
  {
    *http << "<legend>" << label << "</legend>";
  }
  *http << "<table>";
  for (unsigned int i = 0; i < fieldnames.size(); i++)
  {
    BWString fld = fieldnames[i];
    if (fields[fld] != "hidden")
    {
      *http << BWString("<tr><td>") + labels[fld] + ":</td><td> ";
    }
    if (fields[fld] == "select")
    {
      BWList opts = options[fld];
      *http << "<select name='" << fld + "'>";
      for (unsigned int j = 0; j < opts.size(); j++)
      {
        BWList tokens = opts[j].Split("#",2);
        BWString value, desc;
        if (tokens.size() == 2)
        {
          value = tokens[0];
          desc = tokens[1];
        }
        else
        {
          value = opts[j];
          desc = opts[j];
        }
        *http << "<option value='" << value +"'";
        if (curfieldvalues[fld] == value)
        {
          *http << " selected";
        }
        *http << ">" << desc + "</option>";
      }
      *http << "</select><br>";
    }
    /*else if(fields[fld] == "textarea") {
                *http << "<textarea name='" << fld + "'>" + 
                         curfieldvalues[fld] + 
                         "</textarea>";
    }*/
    else if (fields[fld] == "bool")
    {
      *http << "<input type='radio' name='" <<
      fld + "' value='1' checked> Sim" +
      "<input type='radio' name='" + fld +
      "' value='0'>N&atilde;o";
    }
    else
    {
      *http << "<input type='" + fields[fld] + "' name='";
      if (curfieldvalues[fld] == "(null)") {
        *http << fld + "' value=''>";
      }
      else {
        *http << fld + "' value='" + curfieldvalues[fld] + "'>";
      }

      if (fields[fld] != "hidden")
      {
        *http << "<br>";
      }
    }
    if (fields[fld] != "hidden")
    {
      *http << "</td></tr>";
    }
  }
  *http << "<tr><td></td><td><input type='submit' value='Enviar'></td></tr></table>";
  *http << "<input type='hidden' name='" << fnamepre << "' value='1'>";
  *http << "</fieldset></form>";
}


/*!
    \fn Http::BWForm::Report()
 */
void Http::BWForm::Report()
{
  unsigned int i;
  BWString line;
  *http << "<p>";

  for (i = 0; i < fieldnames.size(); i++)
  {
    line = labels[fieldnames[i]] + ": " + curfieldvalues[fieldnames[i]] + "<br>";
    *http << line;
  }
  *http << "</p>";
}


/*!
    \fn Http::BWForm::Show()
*/
void Http::BWForm::Show()
{
  if (IsFilled())
  {
    Report();
  }
  else
  {
    Ask();
  }
}


/*!
    \fn Http::BWForm::Clear()
 */
void Http::BWForm::Clear()
{
  for (unsigned int i = 0; i < fieldnames.size(); i++)
  {
    curfieldvalues[fieldnames[i]] = BWString();
  }
}


/*!
    \fn Http::BWForm::AddField(const BWString &field, const BWString &tipo, const BWString defvalue)
 */
void Http::BWForm::AddField(const BWString &field, const BWString &tipo, const BWString defvalue)
{
  for (unsigned int i = 0; i < fieldnames.size(); i++)
  {
    if (fieldnames[i] == field)
    {
      return;
    }
  }

  isLoaded = false;
  fieldnames.push_back(field);
  if (tipo != "select")
  {
    curfieldvalues[field] = defvalue;
  }
  else
  {
    BWList tokens = defvalue.Split("|");
    curfieldvalues[field] = tokens[0];
    tokens.erase(tokens.begin());
    options[field] = tokens;
  }
  fields[field] = tipo;
  labels[field] = field;
}


/*!
    \fn Http::BWForm::SetLabel(const BWString &fld, const BWString &label)
 */
void Http::BWForm::SetLabel(const BWString &fld, const BWString &labelp)
{
  labels[fld] = labelp;
}


/*!
    \fn Http::BWForm::loadHttpData()
 */
void Http::BWForm::loadHttpData()
{
  if (isLoaded) return;

  for (unsigned int i = 0; i < fieldnames.size(); i++)
  {
    BWString var;
    var = http->GetParameter(fieldnames[i]);
    if (var != "")
    {
      curfieldvalues[fieldnames[i]] = var;
    }
  }

  isLoaded = true;
}


/*!
    \fn Http::BWForm::SetLabel(const BWString &l)
 */
void Http::BWForm::SetLabel(const BWString &l)
{
  label = l;
}
