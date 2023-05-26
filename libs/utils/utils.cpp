#include <utils.h>
#include <iostream>

using namespace std;

BWString urlencode(const BWString &s)
{
            char putxd[4];
            BWString ret = "";
            unsigned int pos;
            int tchr;
            BWString no_url_encodable(":_.");

            for (pos = 0; pos < s.size(); pos++)
            {
                        tchr = s[pos];
                        if (isalnum(tchr) || no_url_encodable.find(s[pos]) > 0)
                        {
                                if (s[pos] != ' ') {
					ret += s[pos];
				}
				else {
					ret += "+";
				}
                        }
                        else
                        {
                                    sprintf(putxd, "%%%2X", s[pos]);
                                    ret += putxd;
                        }
            }

            return ret;
}

BWString urldecode(const BWString &s)
{
            unsigned int pos;
            BWString ret;
            char chr;
            char hex[3];
            unsigned int hexchar;

            for (pos = 0; pos < s.size(); pos++)
            {
                        chr = s[pos];
                        if (chr == '%')
                        {
                                    hex[0] = s[++pos];
                                    hex[1] = s[++pos];
                                    sscanf(hex, "%2X", &hexchar);
                                    ret += hexchar;
                        }
                        else if (chr == '+')
                        {
                                    ret += ' ';
                        }
                        else
                        {
                                    ret += chr;
                        }
            }

            return ret;
}

BWKeyPair envget(char **envp)
{
            char **ptr = envp;
            BWString key, val, curstr;
            BWList tokens;
            BWKeyPair ret;
            unsigned int i;

            while (ptr)
            {

                        if (!*ptr)
                        {
                                    break;
                        }

                        curstr = *ptr;
                        tokens = curstr.Split("=");
                        key = tokens[0];
                        val = tokens[1];

                        for (i = 2; i < tokens.size(); i++)
                        {
                                    val += "=";
                                    val += tokens[i];
                        }

                        ret[key] = val;
                        ptr++;
            }

            return ret;
}

BWKeyPair getqs(const BWString &qs)
{
            BWString parse, key, val;
            BWList varlist, var;
            BWKeyPair ret;

            if (qs.size() <= 0)
            {
                        return ret;
            }

            unsigned int i;

            varlist = qs.Split("?", 2);

            if (varlist.size() > 1)
            {
                        parse = varlist[1];
            }
            else
            {
                        parse = qs;
            }

            varlist = qs.Split("&");
            for (i = 0; i < varlist.size(); i++)
            {
                        var = varlist[i].Split("=", 2);
                        key = var[0];
                        if (var.size() == 2)
                        {
                                    val = var[1];
                        }
                        else
                        {
                                    val = "";
                        }
                        if (ret[urldecode(key)] == "(null)") {
                                    ret[urldecode(key)] = urldecode(val);
                        }
                        else {
                                    if (val != "") {
                                                ret[urldecode(key)] += ",";
                                                ret[urldecode(key)] += urldecode(val);
                                    }
                        }
            }

            return ret;
}

BWKeyPair load_config(const BWString &cf)
{
            BWKeyPair ret;
            BWString curline, t1, t2;
            BWList tokens;
            fstream ifs(cf.c_str(), fstream::in);

	    if (ifs.fail()) {
	    	throw BWError(BASE_ERROR, "Impossivel abrir arquivo de configuracao: " + cf);
	    }

            while (getline(ifs, curline))
            {
                        if (curline == "")
                        {
                                    continue;
                        }
                        tokens = curline.Split("=", 2);
                        if (tokens.size() < 2)

                        {
                                    throw BWError(STRUCTURAL_ERROR, "Arquivo de config. invalido");
                        }
                        t1 = tokens[0];
                        t2 = tokens[1];
                        ret[t1] = t2;
            }

            return ret;
}

