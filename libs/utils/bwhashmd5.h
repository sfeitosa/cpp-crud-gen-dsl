 //
// C++ Interface: bwhashmd5
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, BeanWeb (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef BWHASHMD5_H
#define BWHASHMD5_H

#include <fstream>
#include <bwhash.h>

using std::fstream;

class BWHashMd5: public BWHash {
    public:
        BWHashMd5();
        ~BWHashMd5();

        BWString HashString(const BWString &);
        BWString HashFile(const BWString &);

    private:
        BWString getStrHash(unsigned char *);
};

#endif // BWHASHMD5_H
