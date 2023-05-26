//
// C++ Implementation bwaccessprofile.h
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2014
// 
// Created on: 15/01/2014
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWACCESSPROFILE_H_
#define BWACCESSPROFILE_H_

#include <bwdb.h>
#include <bwstring.h>

namespace CGI {

class BWAccessProfile {
    public:
        BWAccessProfile(int user, const BWString &form, BWDb *db);
        ~BWAccessProfile();

        bool CanAccess();
        bool CanCreate();
        BWString CanList();
        BWString CanEdit();
        BWString CanDelete();

    private:
        int m_user;
        BWString m_form;
        BWDb *m_db;
};

} /* namespace cgi */

#endif /* BWACCESSPROFILE_H_ */
