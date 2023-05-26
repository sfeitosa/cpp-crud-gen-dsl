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
