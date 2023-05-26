/*
 * bwhtmlnav.h
 *
 *  Created on: 26/10/2013
 *      Author: Samuel da Silva Feitosa
 */

#ifndef BWHTMLNAV_H_
#define BWHTMLNAV_H_

#include <bwhttp.h>
#include <bwkeypair.h>
#include <bwlist.h>
#include <bwtemplate.h>
#include <bwconfig.h>
#include <fstream>

namespace CGI {

#define _BW_HTML_NAVBAR BW_ROOT_DIR "var/www/html/templates/navbar.html"
#define _BW_HTML_NAVBAR_HEADER BW_ROOT_DIR "var/www/html/templates/navbar_header.html"
#define _BW_HTML_NAVBAR_ITEM BW_ROOT_DIR "var/www/html/templates/navbar_item.html"
#define _BW_HTML_NAVBAR_LIST BW_ROOT_DIR "var/www/html/templates/navbar_list.html"
#define _BW_HTML_NAVBAR_DROPDOWN BW_ROOT_DIR "var/www/html/templates/navbar_dropdown.html"

class BWHtmlNav {
public:
    BWHtmlNav(const BWString &);
    BWHtmlNav(BWConfig &);
    virtual ~BWHtmlNav();
    BWString GetText();
private:
    void init(BWConfig &);
    BWString generateItens(const BWString &, const BWString &prefix = "");
    BWString genItemDropdown(const BWString &, const BWString &prefix = "");
    BWConfig m_cfg;
    BWString m_text;
    BWTemplate m_menu;
    BWTemplate m_header;
    BWTemplate m_list;
    BWTemplate m_item;
    //BWTemplate m_dropdown;
};

} /* namespace cgi */

#endif /* BWHTMLNAV_H_ */
