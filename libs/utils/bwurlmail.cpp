#include "bwurlmail.h"

BWUrlMail::BWUrlMail() {
    m_current = 0;
    init();
}

BWUrlMail::~BWUrlMail() {
}

void BWUrlMail::SetCredentials(const BWString &user, const BWString &pass) {
    m_username = user;
    m_password = pass;
}

void BWUrlMail::SetUrlServer(const BWString &url) {
    m_url = url;
}

void BWUrlMail::SetSender(const BWString &from) {
    m_sender = from;
}

void BWUrlMail::AddReceiver(const BWString &email) {
    m_receivers.push_back(email);
}

void BWUrlMail::AddReceiver(BWList l) {
    for (unsigned int i = 0; i < l.size(); i++) {
        AddReceiver(l[i]);
    }
}

void BWUrlMail::SetSubject(const BWString &subj) {
    m_subject = subj;
}

void BWUrlMail::SetBodyMsg(const BWString &msg) {
    m_bodymsg = msg;
}

void BWUrlMail::AppendBodyMsg(const BWString &msg) {
    m_bodymsg += msg;
}

void BWUrlMail::Send() {
    struct curl_slist *recipients = NULL;
    CURLcode res;

    prepareMsg();

    curl_easy_setopt(m_curl, CURLOPT_USERNAME, m_username.c_str());
    curl_easy_setopt(m_curl, CURLOPT_PASSWORD, m_password.c_str());
    curl_easy_setopt(m_curl, CURLOPT_URL, m_url.c_str());
    curl_easy_setopt(m_curl, CURLOPT_MAIL_FROM, m_sender.c_str());

    for (unsigned int i = 0; i < m_receivers.size(); i++) {
        recipients = curl_slist_append(recipients, m_receivers[i].c_str());
    }

    curl_easy_setopt(m_curl, CURLOPT_MAIL_RCPT, recipients);

    curl_easy_setopt(m_curl, CURLOPT_READFUNCTION, send_callback);
    curl_easy_setopt(m_curl, CURLOPT_READDATA, this);
    curl_easy_setopt(m_curl, CURLOPT_UPLOAD, 1L);

    // curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);

    res = curl_easy_perform(m_curl);

    if (res != CURLE_OK) {
        throw BWError(APP_ERROR, "Erro ao enviar e-mail");
    }

    curl_slist_free_all(recipients);
    curl_easy_cleanup(m_curl);
}

BWList BWUrlMail::GetRawData() {
    return m_rawmsg;
}

void BWUrlMail::init(void) {
    curl_global_init(CURL_GLOBAL_ALL);

    m_curl = curl_easy_init();

    if (!m_curl) {
        throw BWError(BASE_ERROR, "Erro ao iniciar objeto CURL");
    }
}

void BWUrlMail::prepareMsg() {
    // Remetente da mensagem
    m_rawmsg.push_back("From: " + m_sender + "\r\n");
    // Destinatários
    for (unsigned int i = 0; i < m_receivers.size(); i++) {
        if (i == 0) {
            m_rawmsg.push_back("To: " + m_receivers[i] + "\r\n");
        }
        else {
            m_rawmsg.push_back("Cc: " + m_receivers[i] + "\r\n");
        }
    }
    // Assunto
    m_rawmsg.push_back("Subject: " + m_subject + "\r\n");
    // empty line to divide headers from body, see RFC5322
    m_rawmsg.push_back("\r\n");
    // Corpo da mensagem
    m_rawmsg.push_back(m_bodymsg + "\r\n");
}

size_t BWUrlMail::send_callback(void *ptr, size_t size,
                size_t nmemb, void *usrp) {

    BWString curline;
    BWList alldata;

    if (size == 0 || nmemb == 0 || (size * nmemb) < 1) {
        return 0;
    }

    if (!ptr || !usrp) {
        throw BWError(BASE_ERROR, "Ponteiro de dados invalidos");
    }

    BWUrlMail *obj = static_cast<BWUrlMail *>(usrp);

    alldata = obj->GetRawData();

    if (alldata.size() - 1 < obj->m_current) {
        return 0;
    }

    curline = alldata[obj->m_current];

    memcpy(ptr, curline.c_str(), curline.length());
    obj->m_current++;

    return curline.length();
}
