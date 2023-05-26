#include "bwupdater.h"

BWUpdater::BWUpdater() {
}

BWUpdater::BWUpdater(BWConfig &cfg, const BWString &swname, 
           const BWString &swversion) {
	SetInfo(swname, swversion);
	SetConfig(cfg);
}

void BWUpdater::SetInfo(const BWString &swname, 
						const BWString &swversion) {
    m_swname = swname;
    m_swversion = swversion;
}

void BWUpdater::SetConfig(BWConfig &cfg) {
	m_cfg = cfg;
}

BWUpdater::~BWUpdater() {
}

bool BWUpdater::HasUpdates() {
    BWString url = "http://" + m_cfg["ip"] + "/bw/updater.cgi";
    BWUrlHandler curl;
    BWString resp;
	
	url += "?swname=";
	url += m_swname;
	url += "&swversion=";
	url += m_swversion;

    curl.SetUrl(url);
    resp = curl.Get();

    cout << resp << endl;

    m_updinfo.FromString(resp, "=", "\n", "\""); 

    cout << m_updinfo.ToString() << endl;

    if (m_updinfo[m_swname + ".version"] != "" && 
	    m_updinfo[m_swname + ".version"] != "(null)" && 
	    m_swversion != m_updinfo[m_swname + ".version"]) {
        return true;
    }

    return false;    
}

bool BWUpdater::DownloadUpdate() {
    BWUrlHandler curl;
	BWHashMd5 md5;
    BWString fout = BW_TMPDIR;
	BWString hash = m_updinfo[m_swname + ".hash"];
	
	if (m_updinfo[m_swname + ".exec"] == "(null)" ||
		m_updinfo[m_swname + ".exec"] == "") {
		
		m_updinfo[m_swname + ".exec"] = "download_" + get_date("%Y%m%d");
		
	}

    fout += m_updinfo[m_swname + ".exec"];
	
	// test if file already exists
	if (access(fout.c_str(), 0) == 0) {	
		if (hash != "(null)" && hash != "") {			
			// check checksum
			if (hash.ToUpper() == md5.HashFile(fout).ToUpper()) {
				// file already exists and is the same
				return true;
			}
		}
		else {
			// file already exists
			return true;
		}
	}
	
	// Sem URL para baixar atualizacao
	if (m_updinfo[m_swname + ".url"] == "" ||
	    m_updinfo[m_swname + ".url"] == "(null)") {
		return false;
	}

    curl.SetUrl(m_updinfo[m_swname + ".url"]);
    curl.GetToFile(fout);

    // check checksum...
	if (hash != "(null)" && hash != "") {
		// wrong file - delete it
		if (access(fout.c_str(), 0) == 0 && 
			hash.ToUpper() != md5.HashFile(fout).ToUpper()) {
			unlink(fout.c_str());
			
			return false;
		}
	}
	
	return true;
}

BWString BWUpdater::GetEvalTime() {
    return m_updinfo[m_swname + ".run"];
}

void BWUpdater::PrepareStartupInstall() {
    BWKeyPair stinfo;
	BWString fname = BW_TMPDIR "upd_start";
	BWString fexec = BW_TMPDIR;

	fexec += m_updinfo[m_swname + ".exec"];
	
	stinfo["software"] = m_swname;
	stinfo["version"] = m_updinfo[m_swname + ".version"];
	stinfo["exec"] = fexec;
	stinfo["params"] = m_updinfo[m_swname + ".params"];
	
	stinfo.ToFile(fname, "=", "\n", "\"", "");
}

void BWUpdater::InstallUpdate() {
	BWProcess proc;
	
	try {
		if (m_updinfo["exec"] != "(null)" && m_updinfo["exec"] != "") {
			proc.SetPath(m_updinfo["exec"]);
			
			if (m_updinfo["params"] != "(null)") {
				proc.ExecProgram(m_updinfo["params"]);
			}
			else {
				proc.ExecProgram();
			}
		}	
	}
	catch (BWError &e) {
		m_log(BWLog::MSG_ERROR) << e.GetErrorMessage() << bwlog::endl;
	}
}
