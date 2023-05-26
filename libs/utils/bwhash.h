 //
// C++ Interface: bwhash
//
// Description:
//
//
// Author: Samuel da Silva Feitosa <samuel@beanweb.com.br>, BeanWeb (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef BWHASH_H
#define BWHASH_H

#include <bwstring.h>

class BWHash {
    public:
        BWHash();
        virtual ~BWHash();

        virtual BWString HashString(const BWString &) = 0;
        virtual BWString HashFile(const BWString &) = 0;
};

#endif // BWHASH_H
