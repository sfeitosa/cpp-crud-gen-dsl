/*
 * bwhtmlnav.cpp
 *
 *  Created on: 26/10/2013
 *      Author: Samuel da Silva Feitosa
 */

#include "bwhtmlnav.h"

namespace CGI {


BWHtmlNav::BWHtmlNav(const BWString &file) {
    BWConfig cfg;
    cfg.Load(file);

    init(cfg);
}

BWHtmlNav::BWHtmlNav(BWConfig &cfg) {
    init(cfg);
}

BWHtmlNav::~BWHtmlNav() {
}

BWString BWHtmlNav::GetText() {
    return m_text;
}

void BWHtmlNav::init(BWConfig &cfg) {
    m_cfg = cfg;
    BWString itens;

    m_menu.FromFile(_BW_HTML_NAVBAR);
    m_header.FromFile(_BW_HTML_NAVBAR_HEADER);
    m_list.FromFile(_BW_HTML_NAVBAR_LIST);
    m_item.FromFile(_BW_HTML_NAVBAR_ITEM);

    m_header.SetAttribute("VENDOR", "BEANWEB");
    m_header.SetAttribute("LOGO_HREF", m_cfg["logo_href"]);
    m_header.SetAttribute("LOGO_IMG", m_cfg["logo_img"]);
    itens = generateItens(m_cfg["navbar"]);
    m_list.SetAttribute("ITENS", itens);

    m_menu.SetAttribute("HEADER", m_header.GetText());
    m_menu.SetAttribute("LIST", m_list.GetText());

    m_text = m_menu.GetText();
}

BWString BWHtmlNav::generateItens(const BWString &list, const BWString &prefix) {
	BWList itens;
	BWString htmlItem;

	itens = list.Split(",");

	for (unsigned int i = 0; i < itens.size(); i++) {
		htmlItem += genItemDropdown(itens[i], prefix);
	}

	return htmlItem;
}

BWString BWHtmlNav::genItemDropdown(const BWString &it, const BWString &prefix) {
	BWTemplate dropdown;
	BWString html;
	BWString redir;
	BWString item = prefix + it;

	dropdown.FromFile(_BW_HTML_NAVBAR_DROPDOWN);

	if (it == "-") {
		html = "<li class='divider'></li>";
	}
	else if (m_cfg[item + ".type"] == "header") {
		html  = "<li class='dropdown-header'>";
		html += m_cfg[item + ".descr"];
		html += "</li>";
	}
	else if (m_cfg[item + ".type"] == "item") {
		if (m_cfg[item + ".link"] != "" && m_cfg[item + ".link"] != "(null)") {
			m_item.SetAttribute("CLASS", "bwlink");
		    m_item.SetAttribute("LINK", m_cfg[item + ".link"]);
		}
		else if (m_cfg[item + ".form"] != "" && m_cfg[item + ".form"] != "(null)") {
            redir = "?cad=" + item + "&__action_name=create";
            m_item.SetAttribute("CLASS", "bwmodal");
            m_item.SetAttribute("LINK", redir);
		}
		else {
			redir = "?cad=" + item;
			m_item.SetAttribute("CLASS", "bwlink");
			m_item.SetAttribute("LINK", redir);
		}
		m_item.SetAttribute("DESCR", m_cfg[item + ".descr"]);
		html = m_item.GetText();
	}
	else if (m_cfg[item + ".type"] == "dropdown") {
		dropdown.SetAttribute("DROPDOWN_CLASS", "dropdown");
		dropdown.SetAttribute("DESCR", m_cfg[item + ".descr"] + " <b class='caret'>");
		html = generateItens(m_cfg[item + ".itens"], item + ".itens.");
		dropdown.SetAttribute("ITENS", html);
		html = dropdown.GetText();
	}
	else if (m_cfg[item + ".type"] == "submenu") {
		dropdown.SetAttribute("DROPDOWN_CLASS", "dropdown dropdown-submenu");
		dropdown.SetAttribute("DESCR", m_cfg[item + ".descr"]);
		html = generateItens(m_cfg[item + ".itens"], item + ".itens.");
		dropdown.SetAttribute("ITENS", html);
		html = dropdown.GetText();
	}
	else {
		html = "";
	}

	return html;
}

} /* namespace CGI */
