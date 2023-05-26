//
// C++ Implementation: bwreporthtmlform
//
// Description: 
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bwreporthtmlform.h"

namespace CGI {

BWReportHtmlForm::BWReportHtmlForm(const BWString &file, BWHttp *channel, BWDb &db)
 : BWHtmlForm(file, channel)
{
    init(db);
}

BWReportHtmlForm::BWReportHtmlForm(BWConfig &cfg, BWHttp *channel, BWDb &db)
 : BWHtmlForm(cfg, channel)
{
    init(db);
}


BWReportHtmlForm::~BWReportHtmlForm()
{
}


}

/*!
    \fn CGI::BWReportHtmlForm::GetFieldText(const BWString &field)
 */
BWString CGI::BWReportHtmlForm::GetFieldText(const BWString &field)
{
    BWString idfield = "id";
    BWString descrfield = "descr";
    BWString tabname = bw_config[field + ".table"];
    BWDbRegistry *reg;

    if (tabname != "(null)") {
        if (!m_db) {
            throw BWError(CGI_ERROR, "Recurso de banco de dados nulo");
        }

        reg = new BWDbRegistry(*m_db,
                               tabname);

        if (bw_config[field + ".idfield"] != "(null)") {
            idfield = bw_config[field + ".idfield"];
        }

        if (bw_config[field + ".descrfield"] != "(null)") {
            descrfield = bw_config[field + ".descrfield"];
        }

        if (bw_config[field + ".name"] == "(null)") {
            bw_config[field + ".name"] = field;
        }

        reg->AddField(idfield);
        reg->AddField(descrfield);

        bw_config[field + ".ids"] = reg->GetFieldOptionIds(tabname, idfield).ToString();
        bw_config[field + ".descrs"] = reg->GetFieldOptionDescrs(tabname, descrfield).ToString();
    }

    return CGI::BWHtmlForm::GetFieldText(field);
}


/*!
    \fn CGI::BWReportHtmlForm::init(BWDb &db)
 */
void CGI::BWReportHtmlForm::init(BWDb &db)
{
    BWLog log;

    m_db = &db;
    bw_btn_ok = "Gerar";
    m_randcode = makeRandom();

    bw_config["fields"] += ",";
    bw_config["fields"] += "format";
    bw_fields.push_back("format");

    bw_config["format.type"] = "select";
    bw_config["format.label"] = "Formato";
    bw_config["format.ids"] = "html,pdf";
    bw_config["format.descrs"] = "HTML,PDF";
    bw_config["format.default"] = "pdf";

    if (bw_config["types"] != "(null)") {
        bw_config["fields"] += ",reltype";
        bw_fields.push_back("reltype");
        bw_config["reltype.type"] = "select";
        bw_config["reltype.label"] = "Tipo de relat&oacute;rio";
        bw_config["reltype.ids"] = getTypeIds();
        bw_config["reltype.descrs"] = getTypeDescrs();
        bw_config["reltype.default"] = "det";
    }
}


BWString CGI::BWReportHtmlForm::getTypeIds(void) {
	return bw_config["types"];
}

BWString CGI::BWReportHtmlForm::getTypeDescrs(void) {
	BWList types = bw_config["types"].Split(",");
	BWString strtypes;

	for (unsigned int i = 0; i < types.size(); i++) {
		if (types[i] == "det") {
			strtypes += "Detalhado,";
		}
		else if (types[i] == "sim") {
			strtypes += "Simplificado,";
		}	
		else if (types[i] == "graph") {
			strtypes += "Gr&aacute;fico,";
		}
	}

	strtypes.erase(strtypes.length() - 1, 1);

	return strtypes;
}


/*!
    \fn CGI::BWReportHtmlForm::FillReport()
 */
bool CGI::BWReportHtmlForm::FillReport()
{
    BWKeyPair filters;
    BWString relname;
    BWString ffilter = "/tmp/rel_";
    BWString logfile = "/tmp/rellog.";
    BWString cmdline;

    filters = GetFieldValues();

    relname = makeReportName(bw_config["name"], filters["format"]);

    if (filters["reltype"] != "(null)") {
        filters["relname"] = bw_config["name"] + "_" + filters["reltype"];
    }
    else {
    
        filters["relname"] = bw_config["name"];
    }

    ffilter += m_randcode;
    logfile += m_randcode;

    filters.ToFile(ffilter);

    if (access(ffilter.c_str(), F_OK)) {
        *bw_channel << "<h2>Impossivel criar arquivo de entrada...</h2>";
        return false;
    }

    cmdline = makeCmdLine(relname, ffilter, logfile);

    if (system(cmdline.c_str()) == -1) {
        *bw_channel << "<h2>Erro ao gerar relatorio</h2><br>";
        bw_channel->AppendFile(logfile);
        return false;
    }

    if (access(relname.c_str(), F_OK)) {
        *bw_channel << "<h2>Erro ao gerar relatorio</h2><br>";
        bw_channel->AppendFile(logfile);
        return false;
    }

    sendReport(bw_config["name"], filters["format"]);

    unlink(ffilter.c_str());

    return true;
}

BWString CGI::BWReportHtmlForm::makeReportName(BWString &report,
			BWString &type) {
	BWString filename;
	BWString path = _BW_RELDIR;

	filename  = path + report + "_";
	filename += m_randcode;
	filename += "." + type;

	return filename;
}

BWString CGI::BWReportHtmlForm::makeRandom(void) {
	BWString relnumber = "";

	srandom(time(NULL));

	relnumber.AppendInt(random() % 65535);

	return relnumber;
}

void CGI::BWReportHtmlForm::sendReport(BWString &relname, 
				BWString &format) {
	BWString path = _BW_RELDIR;
	BWString filename;

	filename = relname + "_" + m_randcode + "." + format;

	if (format == "html") {
		bw_channel->SetHeader("Location", 
			BWString("http://") + 
			bw_channel->GetServerVar("HTTP_HOST") +
			"/beanweb/rel/" + filename
			);
	}
	else {
		bw_channel->Clear();
		bw_channel->SetHeader("Content-disposition", 
			BWString("attachment; filename=") + filename);
		bw_channel->AppendFile(path + filename);
	}
}

BWString CGI::BWReportHtmlForm::makeCmdLine(const BWString &relname, 
				const BWString &filters, 
				const BWString &logfile) {
	BWString cmdline;

	cmdline  = _BW_BINDIR;
	cmdline += "bwreports ";
	cmdline += filters + " ";
	cmdline += relname + " ";
	cmdline += logfile;

	return cmdline;
}

