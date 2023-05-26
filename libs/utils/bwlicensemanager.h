//
// C++ Interface: bwlicensemanager
//
// Description: 
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BWLICENSEMANAGER_H
#define BWLICENSEMANAGER_H

#include <bwstring.h>
#include <bwurl.h>
#include <bwerror.h>
#include <bwurlhandler.h>
#include <iostream>
#include <bwdatabase.h>
#include <bwdb.h>
#include <bwresultset.h>
#include <bwnetutils.h>
#include <bwlog.h>

#define BW_LIC_FILE "/srv/beanweb/etc/bwlicense"
#define BW_LIC_CONF "/srv/beanweb/etc/bwlicense.conf"

using namespace std;

/**
Classe responsavel pelo gerenciamento de licenças BEANWEB

	@author Samuel da Silva Feitosa <samuel@beanweb.com.br>
*/
class BWLicenseManager {
    public:
        BWLicenseManager();
        BWLicenseManager(const BWString &);
        ~BWLicenseManager();

        void SetDb(BWDb *db);

        bool Register(const BWString &, const BWString &);
        bool Validate(const BWString &);

        BWString RegisterClient(const BWString &, const BWString &, 
                                const BWString &, const BWString &,
                                const BWString &);
        BWString ValidateClient(const BWString &, const BWString &,
                                const BWString &);
        void WriteLicenseFile(const BWString &, const BWString &, 
                              const BWString &);
        bool ValidateLicenseFile();
        BWString GetEmpresaId();
// colocado como public temporariamente
        BWString generateHashFile(const BWString &, const BWString &,
                                  const BWString &, const BWString &);

    private:
        void init();
        BWString generateHash(const BWString &, const BWString &);
        BWString generateHashResponse(const BWString &, const BWString &);
        bool alreadyRegistered(const BWString &);
        bool validateCode(const BWString &, const BWString &);
        BWString getClientMacAddr(const BWString &);
        //BWString generateHashFile(const BWString &, const BWString &, 
        //                          const BWString &, const BWString &);
        bool validateHashFile(const BWString &, const BWString &, 
                              const BWString &, const BWString &);
        bool validateLicenseDate(const BWString &, const BWString &);
        BWString getClientDescr(const BWString &);
        void writeValidationLog(const BWString &, const BWString &,
                                const BWString &, const BWString &);
        void writeRegistrationLog(const BWString &, const BWString &,
                                const BWString &, const BWString &);

        BWDb *m_db;
        BWConfig m_cfg;
        BWLog m_log;
};

#endif

